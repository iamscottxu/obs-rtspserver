#ifndef _XOP_MACMD5_H
#define _XOP_MACMD5_H

#include <string>
#include "Md5.h"

namespace xop {
class MacMd5 : public Md5 {
public:
	MacMd5();
	virtual ~MacMd5();

	void GetMd5Hash(const unsigned char *data, size_t dataSize,
			unsigned char *outHash);
};
}

#endif
