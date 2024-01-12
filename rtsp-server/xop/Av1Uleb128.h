#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>

namespace xop {
class Av1Uleb128 {
public:
	Av1Uleb128() = delete;

	static std::size_t EncodeUleb128(uint64_t val, uint8_t *data, std::size_t len);
	static std::vector<uint8_t> EncodeUleb128(uint64_t val);

	static std::size_t DecodeUleb128(const uint8_t *data, std::size_t len,
				  uint64_t *val);
	static uint64_t DecodeUleb128(std::vector<uint8_t> data, std::size_t *size);

};
}
