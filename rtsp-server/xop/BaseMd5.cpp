#include "BaseMd5.h"
#include "3rdpart/md5/md5.hpp"

using namespace xop;

BaseMd5::BaseMd5() : Md5() {}

BaseMd5::~BaseMd5() = default;

void BaseMd5::GetMd5Hash(const unsigned char *data, const size_t dataSize,
			 unsigned char *outHash)
{
	md5::md5_state_t state;

	md5_init(&state);
	md5_append(&state, data, dataSize);
	md5_finish(&state, outHash);
}
