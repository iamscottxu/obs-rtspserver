#ifndef XOP_RTSP_PUSHER_H
#define XOP_RTSP_PUSHER_H

#include <mutex>
#include <map>
#include "rtsp.h"

namespace xop {

class RtspConnection;

class RtspPusher : public Rtsp {
public:
	static std::shared_ptr<RtspPusher> Create(EventLoop *loop);
	~RtspPusher() override;

	void AddSession(MediaSession *session);
	void RemoveSession(MediaSessionId session_id);

	int OpenUrl(const std::string &url, int msec = 3000);
	void Close();
	bool IsConnected();

	bool PushFrame(MediaChannelId channelId, const AVFrame &frame);

private:
	friend class RtspConnection;

	explicit RtspPusher(EventLoop *event_loop);
	MediaSession::Ptr LookMediaSession(MediaSessionId session_id) override;

	EventLoop *event_loop_ = nullptr;
	TaskScheduler *task_scheduler_ = nullptr;
	std::mutex mutex_;
	std::shared_ptr<RtspConnection> rtsp_conn_;
	std::shared_ptr<MediaSession> media_session_;
};

}

#endif
