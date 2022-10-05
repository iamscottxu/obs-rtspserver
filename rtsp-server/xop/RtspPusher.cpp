#include "RtspPusher.h"
#include "RtspConnection.h"
#include "net/Logger.h"
#include "net/TcpSocket.h"
#include "net/Timestamp.h"
#include <memory>

using namespace xop;

RtspPusher::RtspPusher(EventLoop *event_loop) : event_loop_(event_loop) {}

RtspPusher::~RtspPusher()
{
	this->Close();
}

std::shared_ptr<RtspPusher> RtspPusher::Create(EventLoop *loop)
{
	std::shared_ptr<RtspPusher> pusher(new RtspPusher(loop));
	return pusher;
}

void RtspPusher::AddSession(MediaSession *session)
{
	std::lock_guard locker(mutex_);
	media_session_.reset(session);
}

void RtspPusher::RemoveSession(MediaSessionId session_id)
{
	std::lock_guard locker(mutex_);
	media_session_ = nullptr; //TODO
}

MediaSession::Ptr RtspPusher::LookMediaSession(MediaSessionId session_id)
{
	return media_session_; //TODO
}

int RtspPusher::OpenUrl(const std::string &url, const int msec)
{
	std::lock_guard lock(mutex_);

	static Timestamp timestamp;
	int timeout = msec;
	if (timeout <= 0) {
		timeout = 10000;
	}

	timestamp.reset();

	if (!this->ParseRtspUrl(url)) {
		LOG_ERROR("rtsp url(%s) was illegal.\n", url.c_str());
		return -1;
	}

	if (rtsp_conn_ != nullptr) {
		std::shared_ptr<RtspConnection> rtspConn = rtsp_conn_;
		SOCKET sockfd = rtspConn->GetSocket();
		task_scheduler_->AddTriggerEvent(
			[sockfd, rtspConn]() { rtspConn->Disconnect(); });
		rtsp_conn_ = nullptr;
	}

	TcpSocket tcpSocket;
	tcpSocket.Create();
	if (!tcpSocket.Connect(rtsp_url_info_.ip, rtsp_url_info_.port,
			       timeout)) {
		tcpSocket.Close();
		return -1;
	}

	task_scheduler_ = event_loop_->GetTaskScheduler();
	rtsp_conn_ = std::make_shared<RtspConnection>(
		tcpSocket.GetSocket(), task_scheduler_, shared_from_this());
	event_loop_->AddTriggerEvent([this]() {
		rtsp_conn_->SendOptions(
			RtspConnection::ConnectionMode::RTSP_PUSHER);
	});

	timeout -= static_cast<int>(timestamp.Elapsed());
	if (timeout < 0) {
		timeout = 1000;
	}

	do {
		Timer::Sleep(100);
		timeout -= 100;
	} while (!rtsp_conn_->IsRecord() && timeout > 0);

	if (!rtsp_conn_->IsRecord()) {
		std::shared_ptr<RtspConnection> rtspConn = rtsp_conn_;
		SOCKET sockfd = rtspConn->GetSocket();
		task_scheduler_->AddTriggerEvent(
			[sockfd, rtspConn]() { rtspConn->Disconnect(); });
		rtsp_conn_ = nullptr;
		return -1;
	}

	return 0;
}

void RtspPusher::Close()
{
	std::lock_guard lock(mutex_);

	if (rtsp_conn_ != nullptr) {
		std::shared_ptr<RtspConnection> rtsp_conn = rtsp_conn_;
		SOCKET sockfd = rtsp_conn->GetSocket();
		task_scheduler_->AddTriggerEvent(
			[sockfd, rtsp_conn]() { rtsp_conn->Disconnect(); });
		rtsp_conn_ = nullptr;
	}
}

bool RtspPusher::IsConnected()
{
	std::lock_guard lock(mutex_);

	if (rtsp_conn_ != nullptr) {
		return (!rtsp_conn_->IsClosed());
	}
	return false;
}

bool RtspPusher::PushFrame(const MediaChannelId channelId, const AVFrame &frame)
{
	std::lock_guard locker(mutex_);
	if (!media_session_ || !rtsp_conn_) {
		return false;
	}

	return media_session_->HandleFrame(channelId, frame);
}
