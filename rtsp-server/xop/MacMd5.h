#ifndef _XOP_MACMD5_H
#define _XOP_MACMD5_H

#include "Md5.h"

namespace xop {
class MacMd5 : public Md5 {
public:
	MacMd5();
	~MacMd5() override;

	void GetMd5Hash(const unsigned char *data, size_t dataSize,
			unsigned char *outHash) override;
};
}

#endif
