#include "NalUnit.h"
#include <cstring>

using namespace xop;
using namespace std;

NalUnit::NalUnit(const uint8_t *data, size_t dataSize)
	: data_(data), data_size_(dataSize)
{
}

size_t NalUnit::GetData(uint8_t **data) const
{
	*data = const_cast<uint8_t *>(data_);
	return data_size_;
}

vector<uint8_t> NalUnit::GetData() const
{
	uint8_t *data = nullptr;
	const auto size = GetData(&data);
	return vector<uint8_t>(data, data + size);
}

size_t NalUnit::CopyData(uint8_t *start, size_t size, size_t skip) const
{
	if (skip > data_size_)
		return 0;
	if (skip + size > data_size_)
		size = data_size_ - skip;
	memcpy(start, data_ + skip, size);
	return size;
}

size_t NalUnit::GetSize() const
{
	return data_size_;
}

std::vector<uint8_t> NalUnit::GetHeader()
{
	uint8_t *data = nullptr;
	const auto size = GetHeader(&data);
	return vector<uint8_t>(data, data + size);
}

std::vector<uint8_t> NalUnit::GetBody()
{
	uint8_t *data = nullptr;
	const auto size = GetBody(&data);
	return vector<uint8_t>(data, data + size);
}

NalUnit *NalUnit::GetNalUnit(const uint8_t *, size_t)
{
	return nullptr;
}
