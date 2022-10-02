#ifndef _XOP_BASE64ENCODE_H
#define _XOP_BASE64ENCODE_H
#include <string>

namespace xop {
std::string Base64Encode(const void *input, const size_t size);
}

#endif
