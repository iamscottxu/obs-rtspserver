#ifndef _XOP_BASEMD5_H
#define _XOP_BASEMD5_H

#include <string>
#include "Md5.h"

namespace xop {
class BaseMd5 : public Md5 {
public:
	BaseMd5();
	virtual ~BaseMd5();

	void GetMd5Hash(const unsigned char *data, size_t dataSize,
			unsigned char *outHash);
};
}

#endif
