// PHZ
// 2018-9-30
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#include "MediaSession.h"
#include "RtpConnection.h"
#include <cstring>
#include <ctime>
#include <map>
#include <forward_list>
#include "net/Logger.h"
#include "net/SocketUtil.h"

using namespace xop;
using namespace std;

std::atomic_uint MediaSession::last_session_id_(1);

MediaSession::MediaSession(std::string url_suffix, uint32_t max_channel_count)
    : session_id_(++last_session_id_)
    , suffix_(url_suffix)
    , media_sources_(max_channel_count)
    , multicast_port_(max_channel_count, 0)
    , has_new_client_(false), _buffer(max_channel_count)
    , max_channel_count_(max_channel_count)
{
}

MediaSession* MediaSession::CreateNew(std::string url_suffix, uint32_t max_channel_count)
{
	return new MediaSession(std::move(url_suffix), max_channel_count);
}

MediaSession::~MediaSession()
{
	if (!multicast_ip_.empty()) {
		MulticastAddr::instance().Release(multicast_ip_);
	}
}

void MediaSession::AddNotifyConnectedCallback(const NotifyConnectedCallback &cb)
{
	_notifyConnectedCallbacks.push_back(cb);
}

void MediaSession::AddNotifyDisconnectedCallback(const NotifyDisconnectedCallback &cb)
{
	_notifyDisconnectedCallbacks.push_back(cb);
}

bool MediaSession::AddSource(MediaChannelId channelId, MediaSource* source)
{
	if (static_cast<uint8_t>(channelId) >= max_channel_count_) return false;
	source->SetSendFrameCallback([this](MediaChannelId channelId, RtpPacket pkt) {
		std::lock_guard lock(map_mutex_);

		std::forward_list<std::shared_ptr<RtpConnection>> clients;
		std::map<int, RtpPacket> packets;

		for (auto iter = clients_.begin(); iter != clients_.end();) {
			if (auto conn = iter->second.lock(); conn == nullptr) {
				clients_.erase(iter++);
			}
			else  {
				if (int id = conn->GetId(); id >= 0) {
					if (packets.find(id) == packets.end()) {
						RtpPacket tmpPkt;
						memcpy(tmpPkt.data.get(), pkt.data.get(), pkt.size);
						tmpPkt.size = pkt.size;
						tmpPkt.last = pkt.last;
						tmpPkt.timestamp = pkt.timestamp;
						tmpPkt.type = pkt.type;
						packets.emplace(id, tmpPkt);
					}
					clients.emplace_front(conn);
				}
				++iter;
			}
		}
        
		int count = 0;
		for(const auto &iter : clients) {
			if (int id = iter->GetId(); id >= 0) {
				if (auto iter2 = packets.find(id); iter2 != packets.end()) {
					int ret = 0;
					count++;
					ret = iter->SendRtpPacket(channelId, iter2->second);
					if (is_multicast_ && ret == 0) {
						break;
					}				
				}
			}					
		}
		return true;
	});

	media_sources_[static_cast<uint8_t>(channelId)].reset(source);
	return true;
}

bool MediaSession::RemoveSource(MediaChannelId channelId)
{
	media_sources_[static_cast<uint8_t>(channelId)] = nullptr;
	return true;
}

bool MediaSession::StartMulticast()
{  
	if (is_multicast_) {
		return true;
	}

	multicast_ip_ = MulticastAddr::instance().GetAddr();
	if (multicast_ip_.empty()) {
		return false;
	}

	std::random_device rd;
	for(uint32_t chn=0;chn< max_channel_count_;chn++)
		multicast_port_[chn] = htons(rd() & 0xfffe);

	is_multicast_ = true;
	return true;
}

std::string MediaSession::GetSdpMessage(std::string ip, std::string sessionName, bool ipv6)
{
	if (media_sources_.empty()) return "";
                
	//std::string ip = NetInterface::GetLocalIPAddress(ipv6);
	char buf[2048] = {0};

	snprintf(buf, sizeof(buf),
			"v=0\r\n"
			"o=- 9%ld 1 IN IP%d %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n" ,
			static_cast<long>(std::time(nullptr)), ipv6 ? 6 : 4 , ip.c_str()); 

	if(sessionName != "") {
		snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), 
				"s=%s\r\n",
				sessionName.c_str());
	}
    
	if(is_multicast_) {
		snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf),
					"a=type:broadcast\r\n"
					"a=rtcp-unicast: reflection\r\n");
	}
		
	for (uint32_t chn=0; chn< max_channel_count_; chn++) {
		if(media_sources_[chn]) {	
			if(is_multicast_) {
				snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), 
						"%s\r\n",
						media_sources_[chn]->GetMediaDescription(multicast_port_[chn]).c_str()); 
                     
				snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), 
						"c=IN IP%d %s/255\r\n",
						ipv6 ? 6 : 4, multicast_ip_.c_str()); 
			}
			else {
				snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), 
						"%s\r\n",
						media_sources_[chn]->GetMediaDescription(0).c_str());
			}
            
			snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), 
					"%s\r\n",
					media_sources_[chn]->GetAttribute().c_str());
                     
			snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf),											
					"a=control:track%d\r\n", chn);	
		}
	}

	return std::string(buf);
}

MediaSource* MediaSession::GetMediaSource(MediaChannelId channelId)
{
	if (static_cast<uint8_t>(channelId) < max_channel_count_) {
		return media_sources_[static_cast<uint8_t>(channelId)].get();
	}

	return nullptr;
}

bool MediaSession::HandleFrame(MediaChannelId channelId, AVFrame frame)
{
	std::lock_guard lock(mutex_);

        if (static_cast<uint8_t>(channelId) < max_channel_count_) {
		media_sources_[static_cast<uint8_t>(channelId)]->HandleFrame(
			channelId, std::move(frame));
	}
	else {
		return false;
	}

	return true;
}

bool MediaSession::AddClient(SOCKET rtspfd, std::shared_ptr<RtpConnection> rtp_conn)
{
	std::lock_guard lock(map_mutex_);

	if(const auto iter = clients_.find (rtspfd); iter == clients_.end()) {
		std::weak_ptr rtp_conn_weak_ptr = rtp_conn;
		clients_.emplace(rtspfd, rtp_conn_weak_ptr);
		for (auto& cb : _notifyConnectedCallbacks)
			cb(session_id_, static_cast<uint32_t>(clients_.size()), rtp_conn->GetIp()); /* 回调通知当前客户端数量 */
        
		has_new_client_ = true;
		return true;
	}
            
	return false;
}

void MediaSession::RemoveClient(SOCKET rtspfd)
{  
	std::lock_guard lock(map_mutex_);

	if (const auto it = clients_.find(rtspfd); it != clients_.end())
    	{
	        if (const auto conn = it->second.lock()) {
                    for (auto& cb : _notifyDisconnectedCallbacks)
                        cb(session_id_, static_cast<uint32_t>(clients_.size()) - 1, conn->GetIp()); /* 回调通知当前客户端数量 */
                }
		clients_.erase(it);
    	}
}

