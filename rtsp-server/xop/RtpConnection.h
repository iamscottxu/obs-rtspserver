// PHZ
// 2018-6-8
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#ifndef XOP_RTP_CONNECTION_H
#define XOP_RTP_CONNECTION_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include "rtp.h"
#include "media.h"
#include "net/Socket.h"
#include "net/TcpConnection.h"

namespace xop
{

class RtspConnection;

struct SockInfo {
	SOCKET fd;
	std::string ip;
	uint16_t port;
};


class RtpConnection final {
public:
    RtpConnection(std::weak_ptr<TcpConnection> rtsp_connection, uint32_t max_channel_count);
    virtual ~RtpConnection();

    void SetClockRate(MediaChannelId channel_id, uint32_t clock_rate)
    { media_channel_info_[static_cast<uint8_t>(channel_id)].clock_rate = clock_rate; }

    void SetPayloadType(MediaChannelId channel_id, uint32_t payload)
    { media_channel_info_[static_cast<uint8_t>(channel_id)].rtp_header.payload = payload; }

    bool SetupRtpOverTcp(MediaChannelId channel_id, uint8_t rtp_channel, uint8_t rtcp_channel);
    bool SetupRtpOverUdp(MediaChannelId channel_id, uint16_t rtp_port, uint16_t rtcp_port);
    bool SetupRtpOverMulticast(MediaChannelId channel_id, std::string ip, uint16_t port);

    uint16_t GetRtpSessionId() const
    { return (uint16_t)((size_t)(this)); }

    uint16_t GetRtpPort(MediaChannelId channel_id) const
    { return local_rtp_port_[static_cast<uint8_t>(channel_id)]; }

    uint16_t GetRtcpPort(MediaChannelId channel_id) const
    { return local_rtcp_port_[static_cast<uint8_t>(channel_id)]; }

    SockInfo GetRtcpSockInfo(MediaChannelId channel_id)
    { return rtcpfd_[static_cast<uint8_t>(channel_id)]; }

	std::string GetIp()
    {
	    const auto conn = rtsp_connection_.lock();
	    return conn ? conn->GetIp() : "";
    }

    bool IsMulticast() const
    { return is_multicast_; }

    bool IsSetup(MediaChannelId channel_id) const
    { return media_channel_info_[static_cast<uint8_t>(channel_id)].is_setup; }

    std::string GetMulticastIp(MediaChannelId channel_id) const;

    void Play();
    void Record();
    void Teardown();

    std::string GetRtpInfo(const std::string& rtsp_url);
    int SendRtpPacket(MediaChannelId channel_id, RtpPacket pkt);

    bool IsClosed() const
    { return is_closed_; }

    int GetId() const;

    bool HasKeyFrame() const
    { return has_key_frame_; }

private:
    friend class RtspConnection;
    friend class MediaSession;
    void SetFrameType(FrameType frameType = FrameType::NONE);
    void SetRtpHeader(MediaChannelId channel_id, RtpPacket pkt);
    int  SendRtpOverTcp(MediaChannelId channel_id, RtpPacket pkt);
    int  SendRtpOverUdp(MediaChannelId channel_id, RtpPacket pkt);

   uint8_t max_channel_count_ = 0;

	std::weak_ptr<TcpConnection> rtsp_connection_;

    TransportMode transport_mode_;
    bool is_multicast_ = false;

	bool is_closed_ = false;
	bool has_key_frame_ = false;

    FrameType frame_type_ = FrameType::NONE;
    std::vector<uint16_t> local_rtp_port_;
    std::vector<uint16_t> local_rtcp_port_;
    std::vector<SockInfo> rtpfd_;
	std::vector<SockInfo> rtcpfd_;

    struct sockaddr_in6 peer_addr_;
    std::vector<struct sockaddr_in6> peer_rtp_addr_;
    std::vector<struct sockaddr_in6> peer_rtcp_sddr_;
    std::vector<MediaChannelInfo> media_channel_info_;

    bool ipv6_;
};

}

#endif
