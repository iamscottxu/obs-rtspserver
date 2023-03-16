
#include "Base64Encode.h"

#include <vector>

extern "C" {
#include "b64/cencode.h"
}

std::string xop::Base64Encode(const void *input, const size_t size)
{
	std::vector<char> buffer(size / 3 * 4 + (size % 3 > 0 ? 4 : 0) + 1);
	base64_encodestate b64encoder;
	base64_init_encodestate(&b64encoder);

	const auto length = base64_encode_block(
		static_cast<const char *>(input), static_cast<int>(size),
		buffer.data(), &b64encoder);
	base64_encode_blockend(buffer.data() + length, &b64encoder);

	return std::string(buffer.cbegin(), buffer.cend() - 1); //TODO
}
