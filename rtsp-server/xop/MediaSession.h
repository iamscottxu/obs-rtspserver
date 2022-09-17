// PHZ
// 2018-6-8
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#ifndef XOP_MEDIA_SESSION_H
#define XOP_MEDIA_SESSION_H

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <random>
#include <cstdint>
#include <map>
#include <unordered_set>
#include "media.h"
#include "H264Source.h"
#include "H265Source.h"
#include "VP8Source.h"
#include "G711ASource.h"
#include "AACSource.h"
#include "MediaSource.h"
#include "net/Socket.h"
#include "net/RingBuffer.h"

namespace xop {

class RtpConnection;

class MediaSession {
public:
	using Ptr = std::shared_ptr<MediaSession>;
	using NotifyConnectedCallback =
		std::function<void(MediaSessionId sessionId,
				   std::string peer_ip, uint16_t peer_port)>;
	using NotifyDisconnectedCallback =
		std::function<void(MediaSessionId sessionId,
				   std::string peer_ip, uint16_t peer_port)>;

	static MediaSession *CreateNew(std::string url_suffix = "live",
				       uint32_t max_channel_count = 2);
	virtual ~MediaSession();

	bool AddSource(MediaChannelId channel_id, MediaSource *source);
	bool RemoveSource(MediaChannelId channel_id);

	bool StartMulticast();

	void
	AddNotifyConnectedCallback(const NotifyConnectedCallback &callback);

	void AddNotifyDisconnectedCallback(
		const NotifyDisconnectedCallback &callback);

	std::string GetRtspUrlSuffix() const { return suffix_; }

	void SetRtspUrlSuffix(const std::string &suffix) { suffix_ = suffix; }

	std::string GetSdpMessage(std::string ip,
				  const std::string &session_name,
				  bool ipv6 = false) const;

	MediaSource *GetMediaSource(MediaChannelId channel_id) const;

	bool HandleFrame(MediaChannelId channel_id, AVFrame frame);

	bool AddClient(SOCKET rtspfd,
		       const std::shared_ptr<RtpConnection> &rtp_conn);
	void RemoveClient(SOCKET rtspfd);

	MediaSessionId GetMediaSessionId() const { return session_id_; }

	uint32_t GetNumClient() const
	{
		return static_cast<uint32_t>(clients_.size());
	}

	uint8_t GetMaxChannelCount() const { return max_channel_count_; }

	bool IsMulticast() const { return is_multicast_; }

	std::string GetMulticastIp(const bool ipv6) const
	{
		return ipv6 ? multicast_ip6_ : multicast_ip_;
	}

	uint16_t GetMulticastPort(MediaChannelId channel_id) const
	{
		if (static_cast<uint8_t>(channel_id) >=
		    multicast_port_.size()) {
			return 0;
		}
		return multicast_port_[static_cast<uint8_t>(channel_id)];
	}

private:
	friend class MediaSource;
	friend class RtspServer;
	MediaSession(std::string url_suffix, uint8_t max_channel_count);

	uint8_t max_channel_count_ = 0;

	MediaSessionId session_id_ = 0;
	std::string suffix_;

	std::vector<std::unique_ptr<MediaSource>> media_sources_;
	std::vector<RingBuffer<AVFrame>> buffer_;

	std::vector<NotifyConnectedCallback> notify_connected_callbacks_;
	std::vector<NotifyDisconnectedCallback> notify_disconnected_callbacks_;
	std::mutex mutex_;
	std::mutex map_mutex_;
	std::map<SOCKET, std::weak_ptr<RtpConnection>> clients_;

	bool is_multicast_ = false;
	std::vector<uint16_t> multicast_port_;
	std::string multicast_ip_;
	std::string multicast_ip6_;
	std::atomic_bool has_new_client_;

	static std::atomic_uint last_session_id_;
};

class MulticastAddr {
public:
	static MulticastAddr &instance()
	{
		static MulticastAddr s_multi_addr;
		return s_multi_addr;
	}

	std::string GetAddr6()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		std::random_device rd;
		char addr6_str[INET6_ADDRSTRLEN];
		for (int n = 0; n <= 10; n++) {
			in6_addr addr6{};
			uint8_t *addr_bytes = addr6.s6_addr;
			addr_bytes[0] = 0xff;
			addr_bytes[1] =        //flgs: |0|R|P|T|
				0x00 << 7 |          //0: reserved
				0x00 << 6 |          //R: default
				0x00 << 5 |          //P: default
				0x01 << 4;           //T: dynamic
			addr_bytes[1] |= 0x02; //scop: Link-Local scope
			//group id
			uint32_t group_id = rd();
			memcpy(addr_bytes + 2, &group_id, 4);
			group_id = rd();
			memcpy(addr_bytes + 6, &group_id, 4);
			group_id = rd();
			memcpy(addr_bytes + 10, &group_id, 4);
			group_id = rd();
			memcpy(addr_bytes + 14, &group_id, 2);
			inet_ntop(AF_INET6, &addr6, addr6_str,
				  INET6_ADDRSTRLEN);
			if (addrs_.find(addr6_str) == addrs_.end()) {
				addrs_.insert(addr6_str);
				break;
			}
		}
		return addr6_str;
	}

	std::string GetAddr6(const in6_addr local_addr6, uint8_t plen)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		std::random_device rd;
		char addr6_str[INET6_ADDRSTRLEN];
		for (int n = 0; n <= 10; n++) {
			in6_addr addr6{};
			uint8_t *addr_bytes = addr6.s6_addr;
			const uint8_t *local_addr_bytes = local_addr6.s6_addr;
			addr_bytes[0] = 0xff;
			addr_bytes[1] =          //flgs: |0|R|P|T|
				0x00 << 7 |            //0: reserved
				0x00 << 6 |            //R: default
				0x01 << 5 |            //P: RFC3306
				0x01 << 4;             //T: dynamic
			addr_bytes[1] |= 0x02;   //scop: Link-Local scope
			addr_bytes[2] = 0x01;    //reserved
			addr_bytes[3] = plen;    //plen
			//network prefix
			const size_t plan_size = plen / 8;
			memcpy(addr_bytes + 4, local_addr_bytes, plan_size);
			const uint8_t plan_remainder = plen % 8;
			if (plan_remainder > 0)
				addr_bytes[4 + plan_size] = local_addr_bytes[plan_size] & (0xff << (8 - plan_remainder));
			//group id
			const uint32_t group_id = rd();
			memcpy(addr_bytes + 13, &group_id, sizeof(uint32_t));
			inet_ntop(AF_INET6, &addr6, addr6_str,
				  INET6_ADDRSTRLEN);
			if (addrs_.find(addr6_str) == addrs_.end()) {
				addrs_.insert(addr6_str);
				break;
			}
		}
		return addr6_str;
	}

	std::string GetAddr()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		std::random_device rd;

		char addr_str[INET_ADDRSTRLEN];
		for (int n = 0; n <= 10; n++) {
			in_addr addr{};
			constexpr uint32_t range = 0xE8FFFFFF - 0xE8000100;
			addr.s_addr = htonl(0xE8000100 + rd() % range);
			inet_ntop(AF_INET, &addr, addr_str, INET_ADDRSTRLEN);
			if (addrs_.find(addr_str) == addrs_.end()) {
				addrs_.insert(addr_str);
				break;
			}
		}
		return addr_str;
	}

	void Release(const std::string &addr)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		addrs_.erase(addr);
	}

private:
	std::mutex mutex_;
	std::unordered_set<std::string> addrs_;
};

}

#endif
