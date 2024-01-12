#include "Av1GetBitContext.h"

using namespace xop;

Av1GetBitContext::Av1GetBitContext(const std::vector<uint8_t>& buffer, bool bit8_size):
	  Av1GetBitContext(buffer.data(), buffer.size(), bit8_size) {

}

Av1GetBitContext::Av1GetBitContext(const uint8_t *buffer, uint64_t size, bool bit8_size)
{
	if (bit8_size) {
		if (size > INT64_MAX / 8 || size < 0)
			size = -1;
		size *= 8;
	}

	if (size >= INT64_MAX - 64 * 8 || size < 0 || !buffer) {
		size = 0;
		buffer = nullptr;
	}

	buffer_ = buffer;
	size_in_bits_ = size;
	size_in_bits_plus8_ = size + 8;
	index_ = 0;
}

uint8_t Av1GetBitContext::GetBit1()
{
	uint64_t index = index_;
	uint8_t result = buffer_[index >> 3];

	result <<= index & 7;
	result >>= 8 - 1;

	if (index_ < size_in_bits_plus8_)
		index++;
	index_ = index;

	return result;
}

uint8_t Av1GetBitContext::GetBits(uint64_t n)
{
	uint8_t out = 0;
	for (uint64_t i = 0; i < n; i++)
		out = (out << 1) | GetBit1();
	return out;
}

void Av1GetBitContext::SkipBits(uint64_t n) {
	GetBits(n);
}

uint64_t Av1GetBitContext::GetBitsCount() const
{
	return this->index_;
}

uint64_t Av1GetBitContext::GetBitsLeft() const
{
	return this->size_in_bits_ - GetBitsCount();
}

void Av1GetBitContext::Uvlc()
{
	uint64_t leading_zeros = 0;

	while (GetBitsLeft()) {
		if (GetBits(1))
			break;
		leading_zeros++;
	}

	if (leading_zeros >= 32)
		return;

	SkipBits(leading_zeros);
}
