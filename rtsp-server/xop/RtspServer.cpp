#include "RtspServer.h"
#include "RtspConnection.h"

using namespace xop;
using namespace std;

RtspServer::RtspServer(EventLoop *loop) : TcpServer(loop) {}

RtspServer::~RtspServer() = default;

std::shared_ptr<RtspServer> RtspServer::Create(EventLoop *loop)
{
	std::shared_ptr<RtspServer> server(new RtspServer(loop));
	return server;
}

MediaSessionId RtspServer::AddSession(MediaSession *session)
{
	std::lock_guard locker(mutex_);

	if (rtsp_suffix_map_.find(session->GetRtspUrlSuffix()) !=
	    rtsp_suffix_map_.end())
		return 0;

	if (session->task_scheduler_.lock() != nullptr)
		return 0;

	session->task_scheduler_ = event_loop_->GetTaskScheduler();
	std::shared_ptr<MediaSession> media_session(session);
	MediaSessionId sessionId = media_session->GetMediaSessionId();
	rtsp_suffix_map_.emplace(media_session->GetRtspUrlSuffix(), sessionId);
	media_sessions_.emplace(sessionId, std::move(media_session));

	return sessionId;
}

void RtspServer::RemoveSession(const MediaSessionId sessionId)
{
	std::lock_guard locker(mutex_);

	if (const auto iter = media_sessions_.find(sessionId);
	    iter != media_sessions_.end()) {
		rtsp_suffix_map_.erase(iter->second->GetRtspUrlSuffix());
		media_sessions_.erase(sessionId);
	}
}

MediaSession::Ptr RtspServer::LookMediaSession(const std::string &suffix)
{
	std::lock_guard locker(mutex_);

	if (const auto iter = rtsp_suffix_map_.find(suffix);
	    iter != rtsp_suffix_map_.end()) {
		const MediaSessionId id = iter->second;
		return media_sessions_[id];
	}

	return nullptr;
}

MediaSession::Ptr RtspServer::LookMediaSession(const MediaSessionId session_id)
{
	std::lock_guard locker(mutex_);

	if (const auto iter = media_sessions_.find(session_id);
	    iter != media_sessions_.end()) {
		return iter->second;
	}

	return nullptr;
}

bool RtspServer::PushFrame(const MediaSessionId session_id,
			   const MediaChannelId channel_id,
			   const AVFrame &frame)
{
	std::shared_ptr<MediaSession> sessionPtr;

	{
		std::lock_guard locker(mutex_);
		if (const auto iter = media_sessions_.find(session_id);
		    iter != media_sessions_.end()) {
			sessionPtr = iter->second;
		} else {
			return false;
		}
	}

	if (sessionPtr != nullptr && sessionPtr->GetNumClient() != 0) {
		return sessionPtr->HandleFrame(channel_id, frame);
	}

	return false;
}

TcpConnection::Ptr RtspServer::OnConnect(SOCKET sockfd)
{
	return std::make_shared<RtspConnection>(
		sockfd, event_loop_->GetTaskScheduler(), shared_from_this());
}
