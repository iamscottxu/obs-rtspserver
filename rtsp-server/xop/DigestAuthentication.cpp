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

DigestAuthentication::DigestAuthentication(std::string realm,
					   std::string username,
					   std::string password)
	: realm_(std::move(realm)),
	  username_(std::move(username)),
	  password_(std::move(password))
{
#if defined(WIN32) || defined(_WIN32)
	md5_ = new CngMd5();
#elif defined(__linux) || defined(__linux__)
	md5_ = new BaseMd5();
#elif defined(__APPLE__) || defined(__MACH__)
	md5_ = new MacMd5();
#else
	md5_ = new BaseMd5();
#endif
}

DigestAuthentication::~DigestAuthentication()
{
	delete md5_;
}

std::string DigestAuthentication::GetNonce() const
{
	std::random_device rd;

	const auto timePoint =
		std::chrono::time_point_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now());
	const uint32_t timestamp =
		static_cast<uint32_t>(timePoint.time_since_epoch().count());

	return md5_->GetMd5HashString(std::to_string(timestamp + rd()));
}

std::string DigestAuthentication::GetResponse(const std::string &nonce,
					      const std::string &cmd,
					      const std::string &url) const
{
	//md5(md5(<username>:<realm> : <password>) :<nonce> : md5(<cmd>:<url>))
	const auto hex1 = md5_->GetMd5HashString(username_ + ":" + realm_ +
						 ":" + password_);
	const auto hex2 = md5_->GetMd5HashString(cmd + ":" + url);
	auto response = md5_->GetMd5HashString(hex1 + ":" + nonce + ":" + hex2);
	return response;
}
