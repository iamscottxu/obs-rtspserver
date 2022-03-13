#ifndef _XOP_CNGMD5_H
#define _XOP_CNGMD5_H

#include <string>
#include "Md5.h"

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#include <bcrypt.h>
#endif

namespace xop {
class CngMd5 : public Md5 {
public:
	CngMd5();
	~CngMd5() override;

	void GetMd5Hash(const unsigned char *data, size_t dataSize,
			unsigned char *outHash) override;

private:
#if defined(WIN32) || defined(_WIN32)
	BCRYPT_ALG_HANDLE hAlgorithm_ = nullptr;
	DWORD cbHash_ = 0, cbHashObject_ = 0;
#endif
};
}

#endif
