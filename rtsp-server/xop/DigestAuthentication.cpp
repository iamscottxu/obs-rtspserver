#include "DigestAuthentication.h"
#if defined(__APPLE__) || defined(__MACH__)
#include <chrono>
#include <random>
#include <CommonCrypto/CommonDigest.h>
#else
#include "md5/md5.hpp"
#endif

#if defined(__APPLE__) || defined(__MACH__)
std::string md5_hash_hex(std::string const & input)
{
        unsigned char result[CC_MD5_DIGEST_LENGTH];
        const char hexValue[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        CC_MD5(input.c_str(), (CC_LONG)input.length(), result);
        char md5Hex[2 * CC_MD5_DIGEST_LENGTH + 1];
        for (size_t i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
                md5Hex[2 * i] = hexValue[((result[i] >> 4) & 0xF)];
                md5Hex[2 * i + 1] = hexValue[(result[i]) & 0x0F];
        }
        md5Hex[2 * CC_MD5_DIGEST_LENGTH] = '\0';
        return md5Hex;
}

std::string generate_nonce()
{
        std::random_device rd;

        auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
        uint32_t timestamp = (uint32_t)timePoint.time_since_epoch().count();

        md5_hash_hex(std::to_string(timestamp + rd()));
}
#else
std::string md5_hash_hex(std::string const & input)
{
	return md5::md5_hash_hex(input);
}

std::string generate_nonce()
{
	return md5::generate_nonce();
}
#endif

using namespace xop;

DigestAuthentication::DigestAuthentication(std::string realm, std::string username, std::string password)
	: m_realm(realm)
	, m_username(username)
	, m_password(password)
{

}

DigestAuthentication::~DigestAuthentication()
{

}

std::string DigestAuthentication::GetNonce()
{
	return generate_nonce();
}

std::string DigestAuthentication::GetResponse(std::string nonce, std::string cmd, std::string url)
{
	//md5(md5(<username>:<realm> : <password>) :<nonce> : md5(<cmd>:<url>))
	auto hex1 = md5_hash_hex(m_username + ":" + m_realm + ":" + m_password);
	auto hex2 = md5_hash_hex(cmd + ":" + url);
	auto response = md5_hash_hex(hex1 + ":" + nonce + ":" + hex2);
	return response;
}

