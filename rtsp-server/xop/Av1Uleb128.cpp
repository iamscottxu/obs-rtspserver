#include "Av1Uleb128.h"
#include "uleb128.h"

using namespace xop;

std::size_t Av1Uleb128::EncodeUleb128(uint64_t val, uint8_t* data, std::size_t len) {
	return bfs::EncodeUleb128(val, data, len);
}

std::vector<uint8_t> Av1Uleb128::EncodeUleb128(uint64_t val)
{
	auto data = std::vector<uint8_t>(8);
	auto size = Av1Uleb128::EncodeUleb128(val, data.data(), data.size());
	data.resize(size);
	return data;
}

std::size_t Av1Uleb128::DecodeUleb128(const uint8_t* data, std::size_t len, uint64_t * val) {
	return bfs::DecodeUleb128(data, len, val);
}

uint64_t Av1Uleb128::DecodeUleb128(std::vector<uint8_t> data, std::size_t* size)
{
	uint64_t val;
	*size = Av1Uleb128::DecodeUleb128(data.data(), data.size(), &val);
	return val;
}
