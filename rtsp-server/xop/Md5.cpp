#include "Md5.h"

using namespace xop;

Md5::Md5() {}

Md5::~Md5() {}

void Md5::GetMd5Hash(const unsigned char *data, size_t dataSize,
			  unsigned char *outHash)
{

}

void Md5::GetMd5Hash(std::string str, unsigned char *outHash)
{
	GetMd5Hash((unsigned char*)str.c_str(), str.length(), outHash);
}

std::string Md5::GetMd5HashString(const unsigned char *data,
				       size_t dataSize)
{
	unsigned char hash[MD5_HASH_LENGTH];
	GetMd5Hash(data, dataSize, hash);
	char hashStr[2 * MD5_HASH_LENGTH + 1];
	for (size_t i = 0; i < MD5_HASH_LENGTH; i++) {
		hashStr[2 * i] = hexValue_[((hash[i] >> 4) & 0xF)];
		hashStr[2 * i + 1] = hexValue_[(hash[i]) & 0x0F];
	}
	hashStr[2 * MD5_HASH_LENGTH] = '\0';
	return hashStr;
}

std::string Md5::GetMd5HashString(std::string str)
{
	return GetMd5HashString((unsigned char*)str.c_str(), str.length());
}
