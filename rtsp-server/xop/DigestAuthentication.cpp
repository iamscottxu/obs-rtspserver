#include "DigestAuthentication.h"

#include <chrono>
#include <string>
#include <random>
#if defined(WIN32) || defined(_WIN32)
#include "CngMd5.h"
#elif defined(__linux) || defined(__linux__)
#include "BaseMd5.h"
#elif defined(__APPLE__) || defined(__MACH__)
#include "MacMd5.h"
#else
#include "BaseMd5.h"
#endif

using namespace xop;

DigestAuthentication::DigestAuthentication(std::string realm, std::string username, std::string password)
	: realm_(realm)
	, username_(username)
	, password_(password)
{
#if defined(WIN32) || defined(_WIN32)
	md5 = new CngMd5();
#elif defined(__linux) || defined(__linux__)
	md5 = new BaseMd5();
#elif defined(__APPLE__) || defined(__MACH__)
	md5 = new MacMd5();
#else
	md5 = new BaseMd5();
#endif
}

DigestAuthentication::~DigestAuthentication()
{
	delete md5;
}

std::string DigestAuthentication::GetNonce()
{
	std::random_device rd;

	auto timePoint =
		std::chrono::time_point_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now());
	uint32_t timestamp = (uint32_t)timePoint.time_since_epoch().count();

	return md5->GetMd5HashString(std::to_string(timestamp + rd()));
}

std::string DigestAuthentication::GetResponse(std::string nonce, std::string cmd, std::string url)
{
	//md5(md5(<username>:<realm> : <password>) :<nonce> : md5(<cmd>:<url>))
	auto hex1 = md5->GetMd5HashString(username_ + ":" + realm_ + ":" +
					  password_);
	auto hex2 = md5->GetMd5HashString(cmd + ":" + url);
	auto response = md5->GetMd5HashString(hex1 + ":" + nonce + ":" + hex2);
	return response;
}

