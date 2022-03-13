#ifndef _XOP_MD5_H
#define _XOP_MD5_H

#include <string>

#define MD5_HASH_LENGTH 16

namespace xop {
class Md5 {
public:
	Md5();
	virtual ~Md5();

	virtual void GetMd5Hash(const unsigned char *data, size_t dataSize,
				unsigned char *outHash);
	void GetMd5Hash(const std::string &str, unsigned char *outHash);
	std::string GetMd5HashString(const unsigned char *data,
				     size_t dataSize);
	std::string GetMd5HashString(const std::string &str);

private:
	const char hex_value_[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
				     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
};
}

#endif
