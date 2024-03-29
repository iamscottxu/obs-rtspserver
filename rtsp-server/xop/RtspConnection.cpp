// PHZ
// 2018-6-10
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#include <utility>
#include "RtspConnection.h"
#include "RtspServer.h"
#include "MediaSession.h"
#include "MediaSource.h"
#include "net/SocketUtil.h"

#define USER_AGENT "-_-"
#define RTSP_DEBUG 0
#define MAX_RTSP_MESSAGE_SIZE 2048

using namespace xop;
using namespace std;

RtspConnection::RtspConnection(const SOCKET sockfd,
                               std::shared_ptr<TaskScheduler> task_scheduler,
                               const std::shared_ptr<Rtsp> &rtsp)
	: TcpConnection(sockfd, std::move(task_scheduler)),
	  alive_count_(1),
	  rtsp_(rtsp)
	  //, rtp_channel_(new Channel(sockfd))
	  ,
	  rtsp_request_(new RtspRequest),
	  rtsp_response_(new RtspResponse)
{
	this->SetReadCallback([this](Weak /*conn*/,
	                             BufferReader &buffer) { return this->OnRead(buffer); });

	this->SetCloseCallback([this](const Weak & /*conn*/) {
		this->OnClose();
	});

	/*rtp_channel_->SetReadCallback([this]() { this->HandleRead(); });
	rtp_channel_->SetWriteCallback([this]() { this->HandleWrite(); });
	rtp_channel_->SetCloseCallback([this]() { this->HandleClose(); });
	rtp_channel_->SetErrorCallback([this]() { this->HandleError(); });*/

	if (rtsp->has_auth_info_) {
		has_auth_ = false;
		auth_info_.reset(new DigestAuthentication(
			rtsp->realm_, rtsp->username_, rtsp->password_));
	}
}

RtspConnection::~RtspConnection() = default;

bool RtspConnection::OnRead(BufferReader &buffer)
{
	KeepAlive();

	if (const size_t size = buffer.ReadableBytes(); size <= 0) {
		return false; //close
	}

	if (conn_mode_ == ConnectionMode::RTSP_SERVER) {
		if (!HandleRtspRequest(buffer)) {
			return false;
		}
	} else if (conn_mode_ == ConnectionMode::RTSP_PUSHER) {
		if (!HandleRtspResponse(buffer)) {
			return false;
		}
	}

	if (buffer.ReadableBytes() > MAX_RTSP_MESSAGE_SIZE) {
		buffer.RetrieveAll();
	}

	return true;
}

void RtspConnection::OnClose()
{
	if (session_id_ != 0) {
		if (const auto rtsp = rtsp_.lock()) {
			if (const MediaSession::Ptr media_session =
				    rtsp->LookMediaSession(session_id_)) {
				media_session->RemoveClient(this->GetSocket(),
							    GetIp(), GetPort());
			}
		}
	}

	for (auto iter = rtcp_channels_.begin();
	     iter != rtcp_channels_.end();) {
		if (auto channel = iter->second; !channel->IsNoneEvent()) {
			GetTaskScheduler()->RemoveChannel(channel);
			rtcp_channels_.erase(iter++);
		} else
			++iter;
	}
}

bool RtspConnection::HandleRtspRequest(BufferReader &buffer)
{
#if RTSP_DEBUG
	string str(buffer.Peek(), buffer.ReadableBytes());
	if (str.find("rtsp") != string::npos ||
	    str.find("RTSP") != string::npos) {
		std::cout << str << std::endl;
	}
#endif

	if (rtsp_request_->ParseRequest(&buffer)) {
		const RtspRequest::Method method = rtsp_request_->GetMethod();
		if (method == RtspRequest::Method::RTCP) {
			HandleRtcp(buffer);
			return true;
		}
		if (!rtsp_request_->GotAll()) {
			return true;
		}

		switch (method) {
		case RtspRequest::Method::OPTIONS:
			HandleCmdOption();
			break;
		case RtspRequest::Method::DESCRIBE:
			HandleCmdDescribe();
			break;
		case RtspRequest::Method::SETUP:
			HandleCmdSetup();
			break;
		case RtspRequest::Method::PLAY:
			HandleCmdPlay();
			break;
		case RtspRequest::Method::TEARDOWN:
			HandleCmdTeardown();
			break;
		case RtspRequest::Method::GET_PARAMETER:
			HandleCmdGetParamter();
			break;
		default:
			break;
		}

		if (rtsp_request_->GotAll()) {
			rtsp_request_->Reset();
		}
	} else {
		return false;
	}

	return true;
}

bool RtspConnection::HandleRtspResponse(BufferReader &buffer)
{
#if RTSP_DEBUG
	string str(buffer.Peek(), buffer.ReadableBytes());
	if (str.find("rtsp") != string::npos ||
	    str.find("RTSP") != string::npos) {
		cout << str << endl;
	}
#endif

	if (rtsp_response_->ParseResponse(&buffer)) {
		switch (rtsp_response_->GetMethod()) {
		case RtspResponse::Method::OPTIONS:
			if (conn_mode_ == ConnectionMode::RTSP_PUSHER) {
				SendAnnounce();
			}
			break;
		case RtspResponse::Method::ANNOUNCE:
		case RtspResponse::Method::DESCRIBE:
			SendSetup();
			break;
		case RtspResponse::Method::SETUP:
			SendSetup();
			break;
		case RtspResponse::Method::RECORD:
			HandleRecord();
			break;
		default:
			break;
		}
	} else {
		return false;
	}

	return true;
}

void RtspConnection::SendRtspMessage(const std::shared_ptr<char> buf,
				     const uint32_t size)
{
#if RTSP_DEBUG
	cout << buf.get() << endl;
#endif

	this->Send(buf, size);
	return;
}

void RtspConnection::HandleRtcp(BufferReader &buffer)
{
	if (const char *peek = buffer.Peek();
	    peek[0] == '$' && buffer.ReadableBytes() > 4) {
		if (const size_t pkt_size = peek[2] << 8 | peek[3];
		    pkt_size + 4 >= buffer.ReadableBytes()) {
			buffer.Retrieve(pkt_size + 4);
		}
	}
}

void RtspConnection::HandleRtcp(const SOCKET sockfd)
{
	if (char buf[1024] = {0}; recv(sockfd, buf, 1024, 0) > 0) {
		KeepAlive();
	}
}

void RtspConnection::HandleCmdOption()
{
	const std::shared_ptr<char> res(new char[2048],
					std::default_delete<char[]>());
	const int size = rtsp_request_->BuildOptionRes(res.get(), 2048);
	this->SendRtspMessage(res, size);
}

void RtspConnection::HandleCmdDescribe()
{
	if (auth_info_ != nullptr && !HandleAuthentication()) {
		return;
	}

	int size;
	const std::shared_ptr<char> res(new char[4096],
					std::default_delete<char[]>());
	MediaSession::Ptr media_session = nullptr;

	const auto rtsp = rtsp_.lock();
	if (rtsp) {
		media_session = rtsp->LookMediaSession(
			rtsp_request_->GetRtspUrlSuffix());
	}

	if (!rtp_conn_ && media_session) {
		if (media_session->IsMulticast())
			rtp_conn_ = media_session->GetMulticastRtpConnection(
				IsIpv6());
		else
			rtp_conn_ = make_shared<RtpConnection>(
				std::dynamic_pointer_cast<RtspConnection>(
					shared_from_this()),
				media_session->GetMaxChannelCount());
	}

	if (!rtsp || !media_session) {
		size = rtsp_request_->BuildNotFoundRes(res.get(), 4096);
	} else {
		session_id_ = media_session->GetMediaSessionId();
		media_session->AddClient(this->GetSocket(), rtp_conn_, GetIp(),
					 GetPort());
		for (uint16_t chn = 0;
		     chn < media_session->GetMaxChannelCount(); chn++) {
			if (MediaSource *source = media_session->GetMediaSource(
				    static_cast<MediaChannelId>(chn));
			    source != nullptr) {
				rtp_conn_->SetClockRate(
					static_cast<MediaChannelId>(chn),
					source->GetClockRate());
				rtp_conn_->SetPayloadType(
					static_cast<MediaChannelId>(chn),
					source->GetPayloadType());
			}
		}

		const auto sdp = media_session->GetSdpMessage(
			SocketUtil::GetSocketIp(GetSocket(), IsIpv6()),
			rtsp->GetVersion(), IsIpv6());
		if (sdp.empty()) {
			size = rtsp_request_->BuildServerErrorRes(res.get(),
								  4096);
		} else {
			size = rtsp_request_->BuildDescribeRes(res.get(), 4096,
							       sdp.c_str());
		}
	}

	SendRtspMessage(res, size);
}

void RtspConnection::HandleCmdSetup()
{
	if (auth_info_ != nullptr && !HandleAuthentication()) {
		return;
	}

	int size;
	const std::shared_ptr<char> res(new char[4096],
					std::default_delete<char[]>());
	MediaChannelId channel_id = rtsp_request_->GetChannelId();
	MediaSession::Ptr media_session = nullptr;

	const auto rtsp = rtsp_.lock();
	if (rtsp && session_id_) {
		media_session = rtsp->LookMediaSession(session_id_);
	}

	if (!rtsp || !media_session) {
		goto server_error;
	}

	if (media_session->IsMulticast()) {
		if (rtsp_request_->GetTransportMode() ==
		    TransportMode::RTP_OVER_MULTICAST) {
			const std::string multicast_ip =
				media_session->GetMulticastIp(IsIpv6());
			const uint16_t port =
				media_session->GetMulticastPort(channel_id);
			const uint16_t session_id =
				rtp_conn_->GetRtpSessionId();
			/*if (!rtp_conn_->SetupRtpOverMulticast(
				    channel_id, multicast_ip, port)) {
				goto server_error;
			}*/
			if (!rtp_conn_->IsSetup(channel_id))
				goto server_error;

			size = rtsp_request_->BuildSetupMulticastRes(
				res.get(), 4096, multicast_ip.c_str(), port,
				session_id);
		} else {
			goto transport_unsupport;
		}
	} else {
		if (rtsp_request_->GetTransportMode() ==
		    TransportMode::RTP_OVER_TCP) {
			const uint8_t rtp_channel =
				rtsp_request_->GetRtpChannel();
			const uint8_t rtcp_channel =
				rtsp_request_->GetRtcpChannel();
			const uint16_t session_id =
				rtp_conn_->GetRtpSessionId();

			rtp_conn_->SetupRtpOverTcp(channel_id, rtp_channel,
						   rtcp_channel);
			size = rtsp_request_->BuildSetupTcpRes(res.get(), 4096,
							       rtp_channel,
							       rtcp_channel,
							       session_id);
		} else if (rtsp_request_->GetTransportMode() ==
			   TransportMode::RTP_OVER_UDP) {
			const uint16_t peer_rtp_port =
				rtsp_request_->GetRtpPort();
			const uint16_t peer_rtcp_port =
				rtsp_request_->GetRtcpPort();
			const uint16_t session_id =
				rtp_conn_->GetRtpSessionId();

			if (rtp_conn_->SetupRtpOverUdp(channel_id,
						       peer_rtp_port,
						       peer_rtcp_port)) {
				auto rtcp_fd = rtp_conn_->GetRtcpfd(channel_id);
				const auto channel =
					make_shared<Channel>(rtcp_fd);
				channel->SetReadCallback([rtcp_fd, this]() {
					this->HandleRtcp(rtcp_fd);
				});
				channel->EnableReading();
				GetTaskScheduler()->UpdateChannel(channel);
				rtcp_channels_[static_cast<uint8_t>(channel_id)] =
					channel;
			} else {
				goto server_error;
			}

			const uint16_t serRtpPort =
				rtp_conn_->GetRtpPort(channel_id);
			const uint16_t serRtcpPort =
				rtp_conn_->GetRtcpPort(channel_id);
			size = rtsp_request_->BuildSetupUdpRes(res.get(), 4096,
							       serRtpPort,
							       serRtcpPort,
							       session_id);
		} else {
			goto transport_unsupport;
		}
	}

	SendRtspMessage(res, size);
	return;

transport_unsupport:
	size = rtsp_request_->BuildUnsupportedRes(res.get(), 4096);
	SendRtspMessage(res, size);
	return;

server_error:
	size = rtsp_request_->BuildServerErrorRes(res.get(), 4096);
	SendRtspMessage(res, size);
}

void RtspConnection::HandleCmdPlay()
{
	if (auth_info_ != nullptr) {
		if (!HandleAuthentication()) {
			return;
		}
	}

	if (rtp_conn_ == nullptr) {
		return;
	}

	conn_state_ = ConnectionState::START_PLAY;
	rtp_conn_->Play();

	const uint16_t session_id = rtp_conn_->GetRtpSessionId();
	const std::shared_ptr<char> res(new char[2048],
					std::default_delete<char[]>());

	const int size = rtsp_request_->BuildPlayRes(res.get(), 2048, nullptr,
						     session_id);
	SendRtspMessage(res, size);
}

void RtspConnection::HandleCmdTeardown()
{
	if (rtp_conn_ == nullptr) {
		return;
	}

	rtp_conn_->Teardown();

	const uint16_t session_id = rtp_conn_->GetRtpSessionId();
	const std::shared_ptr<char> res(new char[2048],
					std::default_delete<char[]>());
	const int size =
		rtsp_request_->BuildTeardownRes(res.get(), 2048, session_id);
	SendRtspMessage(res, size);

	//HandleClose();
}

void RtspConnection::HandleCmdGetParamter()
{
	if (rtp_conn_ == nullptr) {
		return;
	}

	const uint16_t session_id = rtp_conn_->GetRtpSessionId();
	const std::shared_ptr<char> res(new char[2048],
					std::default_delete<char[]>());
	const int size =
		rtsp_request_->BuildGetParamterRes(res.get(), 2048, session_id);
	SendRtspMessage(res, size);
}

bool RtspConnection::HandleAuthentication()
{
	if (auth_info_ != nullptr && !has_auth_) {
		const std::string cmd =
			rtsp_request_->MethodToString[static_cast<uint8_t>(
				rtsp_request_->GetMethod())];

		if (const std::string url = rtsp_request_->GetRtspUrl();
		    !nonce_.empty() &&
		    (auth_info_->GetResponse(nonce_, cmd, url) ==
		     rtsp_request_->GetAuthResponse())) {
			nonce_.clear();
			has_auth_ = true;
		} else {
			const std::shared_ptr<char> res(
				new char[4096], std::default_delete<char[]>());
			nonce_ = auth_info_->GetNonce();
			const int size = rtsp_request_->BuildUnauthorizedRes(
				res.get(), 4096, auth_info_->GetRealm().c_str(),
				nonce_.c_str());
			SendRtspMessage(res, size);
			return false;
		}
	}

	return true;
}

void RtspConnection::SendOptions(const ConnectionMode mode)
{
	const auto rtsp = rtsp_.lock();
	if (!rtsp) {
		HandleClose();
		return;
	}

	const auto media_session = rtsp->LookMediaSession(1);

	if (rtp_conn_ == nullptr) {

		if (media_session->IsMulticast())
			rtp_conn_ = media_session->GetMulticastRtpConnection(IsIpv6());
		else
			rtp_conn_ = make_shared<RtpConnection>(
				std::dynamic_pointer_cast<RtspConnection>(
					shared_from_this()),
				media_session->GetMaxChannelCount());
	}

	conn_mode_ = mode;
	rtsp_response_->SetUserAgent(USER_AGENT);
	rtsp_response_->SetRtspUrl(rtsp->GetRtspUrl().c_str());

	const std::shared_ptr<char> req(new char[2048],
					std::default_delete<char[]>());
	const int size = rtsp_response_->BuildOptionReq(req.get(), 2048);
	SendRtspMessage(req, size);
}

void RtspConnection::SendAnnounce()
{
	MediaSession::Ptr media_session = nullptr;

	const auto rtsp = rtsp_.lock();
	if (rtsp) {
		media_session = rtsp->LookMediaSession(1);
	}

	if (!rtsp || !media_session) {
		HandleClose();
		return;
	}
	session_id_ = media_session->GetMediaSessionId();
	media_session->AddClient(this->GetSocket(), rtp_conn_, GetIp(),
				 GetPort());

	for (uint16_t chn = 0; chn < media_session->GetMaxChannelCount();
	     chn++) {
		if (MediaSource *source = media_session->GetMediaSource(
			    static_cast<MediaChannelId>(chn));
		    source != nullptr) {
			rtp_conn_->SetClockRate(
				static_cast<MediaChannelId>(chn),
				source->GetClockRate());
			rtp_conn_->SetPayloadType(
				static_cast<MediaChannelId>(chn),
				source->GetPayloadType());
		}
	}

	const auto sdp = media_session->GetSdpMessage(
		SocketUtil::GetSocketIp(GetSocket(), IsIpv6()),
		rtsp->GetVersion(), IsIpv6());
	if (sdp.empty()) {
		HandleClose();
		return;
	}

	const std::shared_ptr<char> req(new char[4096],
					std::default_delete<char[]>());
	const int size =
		rtsp_response_->BuildAnnounceReq(req.get(), 4096, sdp.c_str());
	SendRtspMessage(req, size);
}

void RtspConnection::SendDescribe()
{
	const std::shared_ptr<char> req(new char[2048],
					std::default_delete<char[]>());
	const int size = rtsp_response_->BuildDescribeReq(req.get(), 2048);
	SendRtspMessage(req, size);
}

void RtspConnection::SendSetup()
{
	int size = 0;
	const std::shared_ptr<char> buf(new char[2048],
					std::default_delete<char[]>());
	MediaSession::Ptr media_session = nullptr;

	const auto rtsp = rtsp_.lock();
	if (rtsp) {
		media_session = rtsp->LookMediaSession(session_id_);
	}

	if (!rtsp || !media_session) {
		HandleClose();
		return;
	}

	for (uint16_t chn = 0; chn < media_session->GetMaxChannelCount();
	     chn++) {
		if (const auto mediaChannelId =
			    static_cast<MediaChannelId>(chn);
		    media_session->GetMediaSource(mediaChannelId) &&
		    !rtp_conn_->IsSetup(mediaChannelId)) {
			rtp_conn_->SetupRtpOverTcp(mediaChannelId, chn * 2,
						   chn * 2 + 1);
			size = rtsp_response_->BuildSetupTcpReq(buf.get(), 2048,
								chn);
		} else {
			size = rtsp_response_->BuildRecordReq(buf.get(), 2048);
		}
	}

	/*if (media_session->GetMediaSource(MediaChannelId::channel_0) && !rtp_conn_->IsSetup(MediaChannelId::channel_0)) {
		rtp_conn_->SetupRtpOverTcp(MediaChannelId::channel_0, 0, 1);
		size = rtsp_response_->BuildSetupTcpReq(buf.get(), 2048, static_cast<uint8_t>(MediaChannelId::channel_0));
	}
	else if (media_session->GetMediaSource(MediaChannelId::channel_1) && !rtp_conn_->IsSetup(MediaChannelId::channel_1)) {
		rtp_conn_->SetupRtpOverTcp(MediaChannelId::channel_1, 2, 3);
		size = rtsp_response_->BuildSetupTcpReq(buf.get(), 2048, static_cast<uint8_t>(MediaChannelId::channel_1));
	}
	else {
		size = rtsp_response_->BuildRecordReq(buf.get(), 2048);
	}*/

	SendRtspMessage(buf, size);
}

void RtspConnection::HandleRecord()
{
	conn_state_ = ConnectionState::START_PUSH;
	rtp_conn_->Record();
}
