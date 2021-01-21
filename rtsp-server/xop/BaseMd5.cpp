#include "BaseMd5.h"
#include "3rdpart/md5/md5.hpp"

using namespace xop;

BaseMd5::BaseMd5() : Md5() {}

BaseMd5::~BaseMd5() {}

void BaseMd5::GetMd5Hash(const unsigned char *data, size_t dataSize,
			  unsigned char *outHash)
{
	md5::md5_state_t state;

	md5::md5_init(&state);
	md5::md5_append(&state, (md5::md5_byte_t const *)data, dataSize);
	md5::md5_finish(&state, (md5::md5_byte_t *)outHash);
}
