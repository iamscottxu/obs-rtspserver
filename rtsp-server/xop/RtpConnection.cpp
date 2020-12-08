// PHZ
// 2018-9-30
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#include "RtpConnection.h"
#include "RtspConnection.h"
#include "net/SocketUtil.h"

using namespace std;
using namespace xop;

RtpConnection::RtpConnection(std::weak_ptr<TcpConnection> rtsp_connection)
    : rtsp_connection_(rtsp_connection)
    , ipv6_(rtsp_connection.lock()->IsIpv6())
{
	std::random_device rd;

	for(int chn=0; chn<MAX_MEDIA_CHANNEL; chn++) {
		rtpfd_[chn] = 0;
		rtcpfd_[chn] = 0;
		memset(&media_channel_info_[chn], 0, sizeof(media_channel_info_[chn]));
		media_channel_info_[chn].rtp_header.version = RTP_VERSION;
		media_channel_info_[chn].packet_seq = rd()&0xffff;
		media_channel_info_[chn].rtp_header.seq = 0; //htons(1);
		media_channel_info_[chn].rtp_header.ts = htonl(rd());
		media_channel_info_[chn].rtp_header.ssrc = htonl(rd());
	}
}

RtpConnection::~RtpConnection()
{
	for(int chn=0; chn<MAX_MEDIA_CHANNEL; chn++) {
		if(rtpfd_[chn] > 0) {
			SocketUtil::Close(rtpfd_[chn]);
		}

		if(rtcpfd_[chn] > 0) {
			SocketUtil::Close(rtcpfd_[chn]);
		}
	}
}

int RtpConnection::GetId() const
{
	auto conn = rtsp_connection_.lock();
	if (!conn) {
		return -1;
	}
	RtspConnection *rtspConn = (RtspConnection *)conn.get();
	return rtspConn->GetId();
}

bool RtpConnection::SetupRtpOverTcp(MediaChannelId channel_id, uint16_t rtp_channel, uint16_t rtcp_channel)
{
	auto conn = rtsp_connection_.lock();
	if (!conn) {
		return false;
	}

	media_channel_info_[channel_id].rtp_channel = rtp_channel;
	media_channel_info_[channel_id].rtcp_channel = rtcp_channel;
	rtpfd_[channel_id] = conn->GetSocket();
	rtcpfd_[channel_id] = conn->GetSocket();
	media_channel_info_[channel_id].is_setup = true;
	transport_mode_ = RTP_OVER_TCP;

	return true;
}

bool RtpConnection::SetupRtpOverUdp(MediaChannelId channel_id, uint16_t rtp_port, uint16_t rtcp_port)
{
	auto conn = rtsp_connection_.lock();
	if (!conn) {
		return false;
	}

	if (ipv6_ ?
		SocketUtil::GetPeerAddr6(conn->GetSocket(), (struct sockaddr_in6*)&peer_addr_) :
		SocketUtil::GetPeerAddr(conn->GetSocket(), (struct sockaddr_in*)&peer_addr_) < 0) {
		return false;
	}

	media_channel_info_[channel_id].rtp_port = rtp_port;
	media_channel_info_[channel_id].rtcp_port = rtcp_port;

	std::random_device rd;
	for (int n = 0; n <= 10; n++) {
		if (n == 10) {
			return false;
		}
        
		local_rtp_port_[channel_id] = rd() & 0xfffe;
		local_rtcp_port_[channel_id] =local_rtp_port_[channel_id] + 1;

		rtpfd_[channel_id] = ::socket(ipv6_ ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
		if (!SocketUtil::Bind(rtpfd_[channel_id],
				      ipv6_ ? "::0" : "0.0.0.0",
				      local_rtp_port_[channel_id], ipv6_))
		{
			SocketUtil::Close(rtpfd_[channel_id]);
			continue;
		}

		rtcpfd_[channel_id] = ::socket(ipv6_ ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
		if (!SocketUtil::Bind(rtcpfd_[channel_id],
				      ipv6_ ? "::0" : "0.0.0.0",
				      local_rtcp_port_[channel_id], ipv6_))
		{
			SocketUtil::Close(rtpfd_[channel_id]);
			SocketUtil::Close(rtcpfd_[channel_id]);
			continue;
		}

		break;
	}

	SocketUtil::SetSendBufSize(rtpfd_[channel_id], 50*1024);

	if (ipv6_) {
		auto peer_addr = (struct sockaddr_in6 *)&peer_addr_;
		auto peer_rtp_addr = (struct sockaddr_in6 *)&peer_rtp_addr_[channel_id];
		auto peer_rtcp_sddr = (struct sockaddr_in6 *)&peer_rtcp_sddr_[channel_id];
		peer_rtp_addr->sin6_family = AF_INET6;
		peer_rtp_addr->sin6_addr = peer_addr->sin6_addr;
		peer_rtp_addr->sin6_port = htons(media_channel_info_[channel_id].rtp_port);

		peer_rtcp_sddr->sin6_family = AF_INET6;
		peer_rtcp_sddr->sin6_addr = peer_addr->sin6_addr;
		peer_rtcp_sddr->sin6_port = htons(media_channel_info_[channel_id].rtcp_port);
	} else {
		auto peer_addr = (struct sockaddr_in *)&peer_addr_;
		auto peer_rtp_addr = (struct sockaddr_in *)&peer_rtp_addr_[channel_id];
		auto peer_rtcp_sddr = (struct sockaddr_in *)&peer_rtcp_sddr_[channel_id];
		peer_rtp_addr->sin_family = AF_INET;
		peer_rtp_addr->sin_addr = peer_addr->sin_addr;
		peer_rtp_addr->sin_port = htons(media_channel_info_[channel_id].rtp_port);

		peer_rtcp_sddr->sin_family = AF_INET;
		peer_rtcp_sddr->sin_addr = peer_addr->sin_addr;
		peer_rtcp_sddr->sin_port = htons(media_channel_info_[channel_id].rtcp_port);
	}

	media_channel_info_[channel_id].is_setup = true;
	transport_mode_ = RTP_OVER_UDP;

	return true;
}

bool RtpConnection::SetupRtpOverMulticast(MediaChannelId channel_id, std::string ip, uint16_t port)
{
    bool ipv6 = SocketUtil::IsIpv6Address(ip);
    std::random_device rd;
    for (int n = 0; n <= 10; n++) {
		if (n == 10) {
			return false;
		}
       
		local_rtp_port_[channel_id] = rd() & 0xfffe;
		rtpfd_[channel_id] = ::socket(ipv6 ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
		if (!SocketUtil::Bind(rtpfd_[channel_id],
				      ipv6 ? "::0" : "0.0.0.0",
				      local_rtp_port_[channel_id], ipv6)) {
			SocketUtil::Close(rtpfd_[channel_id]);
			continue;
		}

		break;
    }

	media_channel_info_[channel_id].rtp_port = port;

	if (ipv6) {
		auto peer_rtp_addr = (struct sockaddr_in6 *)&peer_rtp_addr_[channel_id];
		peer_rtp_addr->sin6_family = AF_INET6;
		peer_rtp_addr->sin6_port = htons(port);
		inet_pton(AF_INET6, ip.c_str(), &peer_rtp_addr->sin6_addr);
	} else {
		auto peer_rtp_addr = (struct sockaddr_in *)&peer_rtp_addr_[channel_id];
		peer_rtp_addr->sin_family = AF_INET;
		peer_rtp_addr->sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &peer_rtp_addr->sin_addr);
	}

	media_channel_info_[channel_id].is_setup = true;
	transport_mode_ = RTP_OVER_MULTICAST;
	is_multicast_ = true;
	return true;
}

void RtpConnection::Play()
{
	for(int chn=0; chn<MAX_MEDIA_CHANNEL; chn++) {
		if (media_channel_info_[chn].is_setup) {
			media_channel_info_[chn].is_play = true;
		}
	}
}

void RtpConnection::Record()
{
	for (int chn=0; chn<MAX_MEDIA_CHANNEL; chn++) {
		if (media_channel_info_[chn].is_setup) {
			media_channel_info_[chn].is_record = true;
			media_channel_info_[chn].is_play = true;
		}
	}
}

void RtpConnection::Teardown()
{
	if(!is_closed_) {
		is_closed_ = true;
		for(int chn=0; chn<MAX_MEDIA_CHANNEL; chn++) {
			media_channel_info_[chn].is_play = false;
			media_channel_info_[chn].is_record = false;
		}
	}
}

string RtpConnection::GetMulticastIp(MediaChannelId channel_id) const
{
	if (ipv6_) {
		auto peer_rtp_addr = (struct sockaddr_in6 *)&peer_rtp_addr_[channel_id];
		char str[INET6_ADDRSTRLEN] = "::0";
		inet_ntop(AF_INET6, &peer_rtp_addr->sin6_addr, str, sizeof(str));
		return std::string(str);
	} else {
		auto peer_rtp_addr = (struct sockaddr_in *)&peer_rtp_addr_[channel_id];
		char str[INET_ADDRSTRLEN] = "0.0.0.0";
		inet_ntop(AF_INET, &peer_rtp_addr->sin_addr, str, sizeof(str));
		return std::string(str);
	}
}

string RtpConnection::GetRtpInfo(const std::string& rtsp_url)
{
	char buf[2048] = { 0 };
	snprintf(buf, 1024, "RTP-Info: ");

	int num_channel = 0;

	auto time_point = chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now());
	auto ts = time_point.time_since_epoch().count();
	for (int chn = 0; chn<MAX_MEDIA_CHANNEL; chn++) {
		uint32_t rtpTime = (uint32_t)(ts*media_channel_info_[chn].clock_rate / 1000);
		if (media_channel_info_[chn].is_setup) {
			if (num_channel != 0) {
				snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ",");
			}			

			snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
					"url=%s/track%d;seq=0;rtptime=%u",
					rtsp_url.c_str(), chn, rtpTime);
			num_channel++;
		}
	}

	return std::string(buf);
}

void RtpConnection::SetFrameType(uint8_t frame_type)
{
	frame_type_ = frame_type;
	if(!has_key_frame_ && (frame_type == 0 || frame_type == VIDEO_FRAME_I)) {
		has_key_frame_ = true;
	}
}

void RtpConnection::SetRtpHeader(MediaChannelId channel_id, RtpPacket pkt)
{
	if((media_channel_info_[channel_id].is_play || media_channel_info_[channel_id].is_record) && has_key_frame_) {
		media_channel_info_[channel_id].rtp_header.marker = pkt.last;
		media_channel_info_[channel_id].rtp_header.ts = htonl(pkt.timestamp);
		media_channel_info_[channel_id].rtp_header.seq = htons(media_channel_info_[channel_id].packet_seq++);
		memcpy(pkt.data.get()+4, &media_channel_info_[channel_id].rtp_header, RTP_HEADER_SIZE);
	}
}

int RtpConnection::SendRtpPacket(MediaChannelId channel_id, RtpPacket pkt)
{    
	if (is_closed_) {
		return -1;
	}
   
	auto conn = rtsp_connection_.lock();
	if (!conn) {
		return -1;
	}
	RtspConnection *rtsp_conn = (RtspConnection *)conn.get();
	bool ret = rtsp_conn->task_scheduler_->AddTriggerEvent([this, channel_id, pkt] {
		this->SetFrameType(pkt.type);
		this->SetRtpHeader(channel_id, pkt);
		if((media_channel_info_[channel_id].is_play || media_channel_info_[channel_id].is_record) && has_key_frame_ ) {            
			if(transport_mode_ == RTP_OVER_TCP) {
				SendRtpOverTcp(channel_id, pkt);
			}
			else {
				SendRtpOverUdp(channel_id, pkt);
			}
                   
			//media_channel_info_[channel_id].octetCount  += pkt.size;
			//media_channel_info_[channel_id].packetCount += 1;
		}
	});

	return ret ? 0 : -1;
}

int RtpConnection::SendRtpOverTcp(MediaChannelId channel_id, RtpPacket pkt)
{
	auto conn = rtsp_connection_.lock();
	if (!conn) {
		return -1;
	}

	uint8_t* rtpPktPtr = pkt.data.get();
	rtpPktPtr[0] = '$';
	rtpPktPtr[1] = (char)media_channel_info_[channel_id].rtp_channel;
	rtpPktPtr[2] = (char)(((pkt.size-4)&0xFF00)>>8);
	rtpPktPtr[3] = (char)((pkt.size -4)&0xFF);

	conn->Send((char*)rtpPktPtr, pkt.size);
	return pkt.size;
}

int RtpConnection::SendRtpOverUdp(MediaChannelId channel_id, RtpPacket pkt)
{
	//media_channel_info_[channel_id].octetCount  += pktSize;
	//media_channel_info_[channel_id].packetCount += 1;

	int ret = sendto(rtpfd_[channel_id], (const char*)pkt.data.get()+4, pkt.size-4, 0, 
					(struct sockaddr *)&(peer_rtp_addr_[channel_id]),
					sizeof(struct sockaddr_in));
                   
	if(ret < 0) {        
		Teardown();
		return -1;
	}

	return ret;
}
