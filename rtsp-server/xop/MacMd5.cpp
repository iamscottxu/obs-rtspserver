#include "MacMd5.h"

#if defined(__APPLE__) || defined(__MACH__)
#include <CommonCrypto/CommonDigest.h>
#endif

using namespace xop;

MacMd5::MacMd5() : Md5() {}

MacMd5::~MacMd5() {}

void MacMd5::GetMd5Hash(const unsigned char *data, size_t dataSize,
			  unsigned char *outHash)
{
#if defined(__APPLE__) || defined(__MACH__)
	CC_MD5(data, (CC_LONG)dataSize, outHash);
#endif
}
