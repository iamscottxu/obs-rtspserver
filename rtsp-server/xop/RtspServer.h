// PHZ
// 2020-4-2

#ifndef XOP_RTSP_SERVER_H
#define XOP_RTSP_SERVER_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include "net/TcpServer.h"
#include "rtsp.h"

namespace xop
{

class RtspConnection;

class RtspServer : public Rtsp, public TcpServer
{
public:    
    static std::shared_ptr<RtspServer> Create(EventLoop * loop);
    ~RtspServer() override;

    MediaSessionId AddSession(MediaSession* session);
    void RemoveSession(MediaSessionId sessionId);

    bool PushFrame(MediaSessionId session_id, MediaChannelId channel_id, AVFrame frame);

private:
    friend class RtspConnection;

    explicit RtspServer(EventLoop * loop);
    MediaSession::Ptr LookMediaSession(const std::string &suffix) override;
    MediaSession::Ptr LookMediaSession(MediaSessionId session_id) override;
    TcpConnection::Ptr OnConnect(SOCKET sockfd) override;

    std::mutex mutex_;
    std::unordered_map<MediaSessionId, std::shared_ptr<MediaSession>> media_sessions_;
    std::unordered_map<std::string, MediaSessionId> rtsp_suffix_map_;
};

}

#endif

