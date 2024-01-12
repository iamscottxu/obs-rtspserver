#pragma once

#include <cstdint>
#include <vector>

namespace xop {
class Av1GetBitContext {
public:
	explicit Av1GetBitContext(const std::vector<uint8_t>& buffer, bool bit8_size = false);
	Av1GetBitContext(const uint8_t *buffer, uint64_t bit_size, bool bit8_size = false);
	uint8_t GetBit1();
	uint8_t GetBits(uint64_t n);
	void SkipBits(uint64_t n);
	uint64_t GetBitsCount() const;
	uint64_t GetBitsLeft() const;
	void Uvlc();
private:
	const uint8_t *buffer_{};
	uint64_t index_{};
	uint64_t size_in_bits_{};
	uint64_t size_in_bits_plus8_{};
};
}
