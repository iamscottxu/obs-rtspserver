// PHZ
// 2018-6-8
// Scott Xu
// 2020-12-5 Add IPv6 Support.

#ifndef XOP_RTSP_H
#define XOP_RTSP_H

#include <cstdio>
#include <string>
#include "MediaSession.h"
#include "net/Logger.h"

namespace xop {

struct RtspUrlInfo {
	std::string url;
	std::string ip;
	uint16_t port;
	std::string suffix;
};

class Rtsp : public std::enable_shared_from_this<Rtsp> {
public:
	Rtsp() = default;
	virtual ~Rtsp() = default;

	virtual void SetAuthConfig(const std::string realm,
				   const std::string username,
				   const std::string password)
	{
		realm_ = realm;
		username_ = username;
		password_ = password;
		has_auth_info_ = true;

		if (realm_.empty() || username.empty()) {
			has_auth_info_ = false;
		}
	}

	virtual void SetVersion(std::string version) // SDP Session Name
	{
		version_ = std::move(version);
	}

	virtual std::string GetVersion() { return version_; }

	virtual std::string GetRtspUrl() { return rtsp_url_info_.url; }

	bool ParseRtspUrl(const std::string &url)
	{
		char ip[100] = {0};
		char suffix[100] = {0};
		uint16_t port = 0;
#if defined(WIN32) || defined(_WIN32)
		if (sscanf_s(url.c_str() + 7, "[%[^]]]:%hu/%s", ip, 100, &port,
			     suffix, 100) == 3) //IPv6
#else
		if (sscanf(url.c_str() + 7, "[%[^]]]:%hu/%s", ip, &port,
			   suffix) == 3)
#endif
		{
			rtsp_url_info_.port = port;
		}
#if defined(WIN32) || defined(_WIN32)
		else if (sscanf_s(url.c_str() + 7, "[%[^]]]/%s", ip, 100,
				  suffix, 100) == 2)
#else
		else if (sscanf(url.c_str() + 7, "[%[^]]]/%s", ip, suffix) == 2)
#endif
		{
			rtsp_url_info_.port = 554;
		}
#if defined(WIN32) || defined(_WIN32)
		else if (sscanf_s(url.c_str() + 7, "%[^:]:%hu/%s", ip, 100,
				  &port, suffix, 100) == 3) //IPv4, domain
#else
		else if (sscanf(url.c_str() + 7, "%[^:]:%hu/%s", ip, &port,
				suffix) == 3)
#endif
		{
			rtsp_url_info_.port = port;
		}
#if defined(WIN32) || defined(_WIN32)
		else if (sscanf_s(url.c_str() + 7, "%[^/]/%s", ip, 100, suffix,
				  100) == 2)
#else
		else if (sscanf(url.c_str() + 7, "%[^/]/%s", ip, suffix) == 2)
#endif
		{
			rtsp_url_info_.port = 554;
		} else {
			LOG_ERROR("%s was illegal.\n", url.c_str());
			return false;
		}

		rtsp_url_info_.ip = ip;
		rtsp_url_info_.suffix = suffix;
		rtsp_url_info_.url = url;
		return true;
	}

protected:
	friend class RtspConnection;
	virtual MediaSession::Ptr LookMediaSession(const std::string &suffix)
	{
		return nullptr;
	}

	virtual MediaSession::Ptr LookMediaSession(MediaSessionId sessionId)
	{
		return nullptr;
	}

	bool has_auth_info_ = false;
	std::string realm_;
	std::string username_;
	std::string password_;
	std::string version_;
	RtspUrlInfo rtsp_url_info_;
};

}

#endif
