// PHZ
// 2018-5-16
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "RtspMessage.h"
#include "media.h"

using namespace std;
using namespace xop;

bool RtspRequest::ParseRequest(BufferReader *buffer)
{
	if (buffer->Peek()[0] == '$') {
		method_ = Method::RTCP;
		return true;
	}

	bool ret = true;
	while (true) {
		if (state_ == RtspRequestParseState::kParseRequestLine) {
			const char *firstCrlf = buffer->FindFirstCrlf();
			if (firstCrlf != nullptr) {
				ret = ParseRequestLine(buffer->Peek(),
						       firstCrlf);
				buffer->RetrieveUntil(firstCrlf + 2);
			}

			if (state_ ==
			    RtspRequestParseState::kParseHeadersLine) {
				continue;
			}
			break;
		}
		if (state_ == RtspRequestParseState::kParseHeadersLine) {
			const char *lastCrlf = buffer->FindLastCrlf();
			if (lastCrlf != nullptr) {
				ret = ParseHeadersLine(buffer->Peek(),
						       lastCrlf);
				buffer->RetrieveUntil(lastCrlf + 2);
			}
			break;
		}
		if (state_ == RtspRequestParseState::kGotAll) {
			buffer->RetrieveAll();
			return true;
		}
	}

	return ret;
}

bool RtspRequest::ParseRequestLine(const char *begin, const char *end)
{
	const string message(begin, end);
	char method[64] = {0};
	char url[512] = {0};
	char version[64] = {0};

	if (sscanf(message.c_str(), "%s %s %s", method, url, version) != 3) {
		return true;
	}

	string method_str(method);
	if (method_str == "OPTIONS") {
		method_ = Method::OPTIONS;
	} else if (method_str == "DESCRIBE") {
		method_ = Method::DESCRIBE;
	} else if (method_str == "SETUP") {
		method_ = Method::SETUP;
	} else if (method_str == "PLAY") {
		method_ = Method::PLAY;
	} else if (method_str == "TEARDOWN") {
		method_ = Method::TEARDOWN;
	} else if (method_str == "GET_PARAMETER") {
		method_ = Method::GET_PARAMETER;
	} else {
		method_ = Method::NONE;
		return false;
	}

	if (strncmp(url, "rtsp://", 7) != 0) {
		return false;
	}

	// parse url
	uint16_t port = 0;
	char host[64] = {0};
	char suffix[64] = {0};

	if (sscanf(url + 7, "[%[^]]]:%hu/%s", host, &port, suffix) >=
	    2) { //IPv6

	} else if (sscanf(url + 7, "[%[^]]]/%s", host, suffix) >= 1) {
		port = 554;
	} else if (sscanf(url + 7, "%[^:]:%hu/%s", host, &port, suffix) >=
		   2) { //IPv4, domain

	} else if (sscanf(url + 7, "%[^/]/%s", host, suffix) >= 1) {
		port = 554;
	} else {
		return false;
	}

	request_line_param_.emplace("url", make_pair(string(url), 0));
	request_line_param_.emplace("url_host", make_pair(string(host), 0));
	request_line_param_.emplace("url_port",
				    make_pair("", static_cast<uint32_t>(port)));
	request_line_param_.emplace("url_suffix", make_pair(string(suffix), 0));
	request_line_param_.emplace("version", make_pair(string(version), 0));
	request_line_param_.emplace("method", make_pair(move(method_str), 0));

	state_ = RtspRequestParseState::kParseHeadersLine;
	return true;
}

bool RtspRequest::ParseHeadersLine(const char *begin, const char *end)
{
	string message(begin, end);
	if (!ParseCSeq(message)) {
		if (header_line_param_.find("cseq") ==
		    header_line_param_.end()) {
			return false;
		}
	}

	if (method_ == Method::DESCRIBE || method_ == Method::SETUP ||
	    method_ == Method::PLAY) {
		ParseAuthorization(message);
	}

	if (method_ == Method::OPTIONS) {
		state_ = RtspRequestParseState::kGotAll;
		return true;
	}

	if (method_ == Method::DESCRIBE) {
		if (ParseAccept(message)) {
			state_ = RtspRequestParseState::kGotAll;
		}
		return true;
	}

	if (method_ == Method::SETUP) {
		if (ParseTransport(message) && ParseMediaChannel(message)) {
			state_ = RtspRequestParseState::kGotAll;
		}

		return true;
	}

	if (method_ == Method::PLAY) {
		if (ParseSessionId(message)) {
			state_ = RtspRequestParseState::kGotAll;
		}
		return true;
	}

	if (method_ == Method::TEARDOWN) {
		state_ = RtspRequestParseState::kGotAll;
		return true;
	}

	if (method_ == Method::GET_PARAMETER) {
		state_ = RtspRequestParseState::kGotAll;
		return true;
	}

	return true;
}

bool RtspRequest::ParseCSeq(std::string &message)
{
	if (const std::size_t pos = message.find("CSeq");
	    pos != std::string::npos) {
		uint32_t cseq = 0;
		sscanf(message.c_str() + pos, "%*[^:]: %u", &cseq);
		header_line_param_.emplace("cseq", make_pair("", cseq));
		return true;
	}

	return false;
}

bool RtspRequest::ParseAccept(const std::string &message)
{
	if (message.rfind("Accept") == std::string::npos ||
	    message.rfind("sdp") == std::string::npos) {
		return false;
	}

	return true;
}

bool RtspRequest::ParseTransport(std::string &message)
{
	if (std::size_t pos = message.find("Transport");
	    pos != std::string::npos) {
		if ((pos = message.find("RTP/AVP/TCP")) != std::string::npos) {
			transport_ = TransportMode::RTP_OVER_TCP;
			uint16_t rtpChannel = 0, rtcpChannel = 0;
			if (sscanf(message.c_str() + pos,
				   "%*[^;];%*[^;];%*[^=]=%hu-%hu", &rtpChannel,
				   &rtcpChannel) != 2) {
				return false;
			}
			header_line_param_.emplace("rtp_channel",
						   make_pair("", rtpChannel));
			header_line_param_.emplace("rtcp_channel",
						   make_pair("", rtcpChannel));
		} else if ((pos = message.find("RTP/AVP")) !=
			   std::string::npos) {
			uint16_t rtp_port = 0, rtcpPort = 0;
			if (((message.find("unicast", pos)) !=
			     std::string::npos)) {
				transport_ = TransportMode::RTP_OVER_UDP;
				if (sscanf(message.c_str() + pos,
					   "%*[^;];%*[^;];%*[^=]=%hu-%hu",
					   &rtp_port, &rtcpPort) != 2) {
					return false;
				}

			} else if ((message.find("multicast", pos)) !=
				   std::string::npos) {
				transport_ = TransportMode::RTP_OVER_MULTICAST;
			} else {
				return false;
			}

			header_line_param_.emplace("rtp_port",
						   make_pair("", rtp_port));
			header_line_param_.emplace("rtcp_port",
						   make_pair("", rtcpPort));
		} else {
			return false;
		}

		return true;
	}

	return false;
}

bool RtspRequest::ParseSessionId(std::string &message)
{
	const std::size_t pos = message.find("Session");
	if (pos != std::string::npos) {
		uint32_t session_id = 0;
		if (sscanf(message.c_str() + pos, "%*[^:]: %u", &session_id) !=
		    1) {
			return false;
		}
		return true;
	}

	return false;
}

bool RtspRequest::ParseMediaChannel(std::string &message)
{
	if (const auto iter = request_line_param_.find("url");
	    iter != request_line_param_.end()) {
		const std::string url = iter->second.first;
		const std::size_t pos = url.rfind("/track");
		if (pos != std::string::npos) {
			int channel_index;
			if (sscanf(url.c_str() + pos, "/track%d",
				   &channel_index) != 1) {
				return false;
			}
			channel_id_ =
				static_cast<MediaChannelId>(channel_index);
			return true;
		}
	}
	return false;
}

bool RtspRequest::ParseAuthorization(std::string &message)
{
	if (std::size_t pos = message.find("Authorization");
	    pos != std::string::npos) {
		if ((pos = message.find("response=")) != std::string::npos) {
			auth_response_ = message.substr(pos + 10, 32);
			if (auth_response_.size() == 32) {
				return true;
			}
		}
	}

	auth_response_.clear();
	return false;
}

uint32_t RtspRequest::GetCSeq() const
{
	uint32_t cseq = 0;
	if (const auto iter = header_line_param_.find("cseq");
	    iter != header_line_param_.end()) {
		cseq = iter->second.second;
	}

	return cseq;
}

std::string RtspRequest::GetHost() const
{
	if (const auto iter = request_line_param_.find("url_ip");
	    iter != request_line_param_.end()) {
		return iter->second.first;
	}

	return "";
}

std::string RtspRequest::GetRtspUrl() const
{
	if (const auto iter = request_line_param_.find("url");
	    iter != request_line_param_.end()) {
		return iter->second.first;
	}

	return "";
}

std::string RtspRequest::GetRtspUrlSuffix() const
{
	if (const auto iter = request_line_param_.find("url_suffix");
	    iter != request_line_param_.end()) {
		return iter->second.first;
	}

	return "";
}

std::string RtspRequest::GetAuthResponse() const
{
	return auth_response_;
}

uint8_t RtspRequest::GetRtpChannel() const
{
	if (const auto iter = header_line_param_.find("rtp_channel");
	    iter != header_line_param_.end()) {
		return iter->second.second;
	}

	return 0;
}

uint8_t RtspRequest::GetRtcpChannel() const
{
	if (const auto iter = header_line_param_.find("rtcp_channel");
	    iter != header_line_param_.end()) {
		return iter->second.second;
	}

	return 0;
}

uint16_t RtspRequest::GetRtpPort() const
{
	if (const auto iter = header_line_param_.find("rtp_port");
	    iter != header_line_param_.end()) {
		return iter->second.second;
	}

	return 0;
}

uint16_t RtspRequest::GetRtcpPort() const
{
	if (const auto iter = header_line_param_.find("rtcp_port");
	    iter != header_line_param_.end()) {
		return iter->second.second;
	}

	return 0;
}

int RtspRequest::BuildOptionRes(const char *buf, int buf_size) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %u\r\n"
		 "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
		 "\r\n",
		 this->GetCSeq());

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildDescribeRes(const char *buf, const int buf_size,
				  const char *sdp) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %u\r\n"
		 "Content-Length: %d\r\n"
		 "Content-Type: application/sdp\r\n"
		 "\r\n"
		 "%s",
		 this->GetCSeq(), static_cast<int>(strlen(sdp)), sdp);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildSetupMulticastRes(const char *buf, const int buf_size,
					const char *multicast_ip,
					const uint16_t port,
					const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(
		const_cast<char *>(buf), buf_size,
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %u\r\n"
		"Transport: RTP/AVP;multicast;destination=%s;source=%s;port=%u-0;ttl=255\r\n"
		"Session: %u\r\n"
		"\r\n",
		this->GetCSeq(), multicast_ip, this->GetHost().c_str(), port,
		session_id);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildSetupUdpRes(const char *buf, const int buf_size,
				  const uint16_t rtp_chn,
				  const uint16_t rtcp_chn,
				  const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(
		const_cast<char *>(buf), buf_size,
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %u\r\n"
		"Transport: RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu\r\n"
		"Session: %u\r\n"
		"\r\n",
		this->GetCSeq(), this->GetRtpPort(), this->GetRtcpPort(),
		rtp_chn, rtcp_chn, session_id);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildSetupTcpRes(const char *buf, const int buf_size,
				  const uint16_t rtp_chn,
				  const uint16_t rtcp_chn,
				  const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %u\r\n"
		 "Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n"
		 "Session: %u\r\n"
		 "\r\n",
		 this->GetCSeq(), rtp_chn, rtcp_chn, session_id);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildPlayRes(const char *buf, const int buf_size,
			      const char *rtp_info,
			      const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %d\r\n"
		 "Range: npt=0.000-\r\n"
		 "Session: %u; timeout=60\r\n",
		 this->GetCSeq(), session_id);

	if (rtp_info != nullptr) {
		snprintf(const_cast<char *>(buf) + strlen(buf),
			 buf_size - strlen(buf), "%s\r\n", rtp_info);
	}

	snprintf(const_cast<char *>(buf) + strlen(buf), buf_size - strlen(buf),
		 "\r\n");
	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildTeardownRes(const char *buf, const int buf_size,
				  const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %d\r\n"
		 "Session: %u\r\n"
		 "\r\n",
		 this->GetCSeq(), session_id);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildGetParamterRes(const char *buf, const int buf_size,
				     const uint32_t session_id) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 200 OK\r\n"
		 "CSeq: %d\r\n"
		 "Session: %u\r\n"
		 "\r\n",
		 this->GetCSeq(), session_id);

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildNotFoundRes(const char *buf, const int buf_size) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 404 Not Found\r\n"
		 "CSeq: %u\r\n"
		 "\r\n",
		 this->GetCSeq());

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildServerErrorRes(const char *buf, const int buf_size) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 500 Internal Server Error\r\n"
		 "CSeq: %u\r\n"
		 "\r\n",
		 this->GetCSeq());

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildUnsupportedRes(const char *buf, const int buf_size) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 461 Unsupported transport\r\n"
		 "CSeq: %d\r\n"
		 "\r\n",
		 this->GetCSeq());

	return static_cast<int>(strlen(buf));
}

int RtspRequest::BuildUnauthorizedRes(const char *buf, const int buf_size,
				      const char *realm,
				      const char *nonce) const
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RTSP/1.0 401 Unauthorized\r\n"
		 "CSeq: %d\r\n"
		 "WWW-Authenticate: Digest realm=\"%s\", nonce=\"%s\"\r\n"
		 "\r\n",
		 this->GetCSeq(), realm, nonce);

	return static_cast<int>(strlen(buf));
}

bool RtspResponse::ParseResponse(BufferReader *buffer)
{
	if (strstr(buffer->Peek(), "\r\n\r\n") != nullptr) {
		if (strstr(buffer->Peek(), "OK") == nullptr) {
			return false;
		}

		if (char *ptr = strstr(buffer->Peek(), "Session");
		    ptr != nullptr) {
			if (char session_id[50] = {0};
			    sscanf(ptr, "%*[^:]: %s", session_id) == 1)
				session_ = session_id;
		}

		cseq_++;
		buffer->RetrieveUntil("\r\n\r\n");
	}

	return true;
}

int RtspResponse::BuildOptionReq(const char *buf, const int buf_size)
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "OPTIONS %s RTSP/1.0\r\n"
		 "CSeq: %u\r\n"
		 "User-Agent: %s\r\n"
		 "\r\n",
		 rtsp_url_.c_str(), this->GetCSeq() + 1, user_agent_.c_str());

	method_ = Method::OPTIONS;
	return static_cast<int>(strlen(buf));
}

int RtspResponse::BuildAnnounceReq(const char *buf, const int buf_size,
				   const char *sdp)
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "ANNOUNCE %s RTSP/1.0\r\n"
		 "Content-Type: application/sdp\r\n"
		 "CSeq: %u\r\n"
		 "User-Agent: %s\r\n"
		 "Session: %s\r\n"
		 "Content-Length: %d\r\n"
		 "\r\n"
		 "%s",
		 rtsp_url_.c_str(), this->GetCSeq() + 1, user_agent_.c_str(),
		 this->GetSession().c_str(), static_cast<int>(strlen(sdp)),
		 sdp);

	method_ = Method::ANNOUNCE;
	return static_cast<int>(strlen(buf));
}

int RtspResponse::BuildDescribeReq(const char *buf, const int buf_size)
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "DESCRIBE %s RTSP/1.0\r\n"
		 "CSeq: %u\r\n"
		 "Accept: application/sdp\r\n"
		 "User-Agent: %s\r\n"
		 "\r\n",
		 rtsp_url_.c_str(), this->GetCSeq() + 1, user_agent_.c_str());

	method_ = Method::DESCRIBE;
	return static_cast<int>(strlen(buf));
}

int RtspResponse::BuildSetupTcpReq(const char *buf, const int buf_size,
				   const int trackId)
{
	int interleaved[2] = {0, 1};
	if (trackId == 1) {
		interleaved[0] = 2;
		interleaved[1] = 3;
	}

	memset((void *)buf, 0, buf_size); //TODO
	snprintf(
		const_cast<char *>(buf), buf_size,
		"SETUP %s/track%d RTSP/1.0\r\n"
		"Transport: RTP/AVP/TCP;unicast;mode=record;interleaved=%d-%d\r\n"
		"CSeq: %u\r\n"
		"User-Agent: %s\r\n"
		"Session: %s\r\n"
		"\r\n",
		rtsp_url_.c_str(), trackId, interleaved[0], interleaved[1],
		this->GetCSeq() + 1, user_agent_.c_str(),
		this->GetSession().c_str());

	method_ = Method::SETUP;
	return static_cast<int>(strlen(buf));
}

int RtspResponse::BuildRecordReq(const char *buf, const int buf_size)
{
	memset((void *)buf, 0, buf_size); //TODO
	snprintf(const_cast<char *>(buf), buf_size,
		 "RECORD %s RTSP/1.0\r\n"
		 "Range: npt=0.000-\r\n"
		 "CSeq: %u\r\n"
		 "User-Agent: %s\r\n"
		 "Session: %s\r\n"
		 "\r\n",
		 rtsp_url_.c_str(), this->GetCSeq() + 1, user_agent_.c_str(),
		 this->GetSession().c_str());

	method_ = Method::RECORD;
	return static_cast<int>(strlen(buf));
}
