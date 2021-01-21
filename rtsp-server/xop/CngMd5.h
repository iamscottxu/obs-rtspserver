#ifndef _XOP_CNGMD5_H
#define _XOP_CNGMD5_H

#include <string>
#include "Md5.h"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#endif

namespace xop {
class CngMd5 : public Md5 {
public:
	CngMd5();
	~CngMd5();

	void GetMd5Hash(const unsigned char *data, size_t dataSize,
			unsigned char *outHash);

private:
	BCRYPT_ALG_HANDLE hAlgorithm_ = NULL;
};
}

#endif
