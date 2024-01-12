#include "Av1Obu.h"
#include "Av1Uleb128.h"
#include "net/Logger.h"

using namespace xop;
using namespace std;

Av1Obu::Av1Obu(const uint8_t *data, size_t dataSize)
	: data_(data), data_size_(dataSize)
{
	auto headerSize = GetHeaderSize();
	size_t leb128Size = 0;
	auto bodySize = GetBodySizeWithLeb128Size(&leb128Size);
	data_size_ = headerSize + leb128Size + bodySize;
}

size_t Av1Obu::GetData(uint8_t **data)
{
	if (!IsForbiddenBitZero()) return 0;
	*data = const_cast<uint8_t *>(data_);
	return data_size_;
}

vector<uint8_t> Av1Obu::GetData()
{
	uint8_t *data = nullptr;
	const auto size = GetData(&data);
	return vector<uint8_t>(data, data + size);
}

size_t Av1Obu::CopyData(uint8_t *start, size_t size, size_t skip)
{
	if (skip > data_size_)
		return 0;
	if (skip + size > data_size_)
		size = data_size_ - skip;
	memcpy(start, data_ + skip, size);
	return size;
}

size_t Av1Obu::GetSize()
{
	if (!IsForbiddenBitZero()) return 0;
	return data_size_;
}

std::vector<uint8_t> Av1Obu::GetNotHasSizeFieldData()
{
	vector<uint8_t> newData(GetNotHasSizeFieldSize());
	CopyNotHasSizeFieldData(newData.data(), newData.size());
	return newData;
}

size_t Av1Obu::CopyNotHasSizeFieldData(uint8_t *start, size_t size, size_t skip)
{
	auto headerSize = GetHeaderSize();
	size_t headerCopySize = 0;
	if (skip < headerSize) {
		headerCopySize = CopyHeader(start, size, skip);
		if (headerCopySize > 0) start[0] &= 0xfd;
		if (headerCopySize == size) return headerCopySize;
		skip = 0;
	}
	else skip -= headerSize;
	auto bodyCopySize = CopyBody(start + headerCopySize, size - headerCopySize, skip);
	return headerCopySize + bodyCopySize;
}

size_t Av1Obu::GetNotHasSizeFieldSize()
{
	return GetHeaderSize() + GetBodySize();
}

size_t Av1Obu::GetHeader(uint8_t **data)
{
	auto headerSize = GetHeaderSize();
	if (GetData(data) >= headerSize)
		return headerSize;
	return 0;
}

std::vector<uint8_t> Av1Obu::GetHeader()
{
	uint8_t *data = nullptr;
	const auto size = GetHeader(&data);
	return vector<uint8_t>(data, data + size);
}

size_t Av1Obu::CopyHeader(uint8_t *start, size_t size, size_t skip)
{
	auto headerSize = GetHeaderSize();
	if (skip > headerSize) {
		return 0;
	}
	if (skip + size > headerSize) {
		size = headerSize - skip;
	}
	return CopyData(start, size, skip);
}

size_t Av1Obu::GetHeaderSize()
{
	if (GetSize() == 0) return 0;
	else if (HasExtensionHeader()) return 2;
	else return 1;
}

size_t Av1Obu::GetBody(uint8_t **data)
{
	size_t leb128Size = 0;
	auto bodySize = GetBodySizeWithLeb128Size(&leb128Size);
	if (bodySize == 0) return 0;
	auto skip = GetHeaderSize() + leb128Size;
	GetData(data);
	*data += skip;
	return bodySize;
}

std::vector<uint8_t> Av1Obu::GetBody()
{
	uint8_t *data = nullptr;
	const auto size = GetBody(&data);
	return vector<uint8_t>(data, data + size);
}

size_t Av1Obu::CopyBody(uint8_t *start, size_t size, size_t skip)
{
	size_t leb128Size = 0;
	auto bodySize = GetBodySizeWithLeb128Size(&leb128Size);
	if (bodySize == 0) return 0;
	if (skip > bodySize) {
		return 0;
	}
	if (skip + size > bodySize) {
		size = bodySize - skip;
	}
	skip += GetHeaderSize() + leb128Size;
	return CopyData(start, size, skip);
}

size_t Av1Obu::GetBodySize()
{
	return GetBodySizeWithLeb128Size(nullptr);
}

Av1ObuType Av1Obu::GetType()
{
	auto header = GetHeader();
	if (header.empty()) return Av1ObuType::AV1_OBU_RESERVED0;
	return static_cast<Av1ObuType>((header[0] & 0x78) >> 3);
}

bool Av1Obu::HasExtensionHeader()
{
	auto data = GetData();
	if (data.empty()) return false;
	return (data[0] & 0x04) >> 2;
}

bool Av1Obu::HasSizeField()
{
	auto header = GetHeader();
	if (header.empty()) return false;
	bool hasSizeField = (header[0] &0x02) >> 1;
	if (hasSizeField) {
		if(GetSize() - header.size() == 0) {
			LOG_WARNING(
				"[AV1]The size field flag of the obu is 1, "
				"but the size of this obu is only enough to store the obu header.");
			return false;
		}
		return true;
	}
	return false;
}

uint8_t Av1Obu::GetTemporalId()
{
	if (!HasExtensionHeader()) return 0;
	auto header = GetHeader();
	return (header[1] & 0xe0) >> 5;
}

uint8_t Av1Obu::GetSpatialId()
{
	if (!HasExtensionHeader()) return 0;
	auto header = GetHeader();
	return (header[1] & 0x18) >> 3;
}

std::shared_ptr<Av1ObuSequenceHeader> Av1Obu::GetSequenceHeader()
{
	if (GetType() != Av1ObuType::AV1_OBU_SEQUENCE_HEADER) return nullptr;
	return std::shared_ptr<Av1ObuSequenceHeader>(new Av1ObuSequenceHeader(GetBody()));
}

Av1Obu *Av1Obu::GetAv1ObuUnit(const uint8_t *data, size_t dataSize)
{
	return new Av1Obu(data, dataSize);
}

bool Av1Obu::IsForbiddenBitZero()
{
	if (data_size_ == 0) return true;
	if (((data_[0] & 0x80) >> 7) != 0) {
		LOG_WARNING(
			"[AV1]The value of obu forbidden bit is not zero.");
		return false;
	}
	return true;
}

size_t Av1Obu::GetBodySizeWithLeb128Size(size_t *leb128Size)
{
	auto data = GetData();
	auto headerSize = GetHeaderSize();
	auto maxSize = data.size() - headerSize;
	if (leb128Size != nullptr) *leb128Size = 0;
	if (!HasSizeField())
		return maxSize;
	uint64_t value;
	auto _leb128Size = Av1Uleb128::DecodeUleb128(data.data() + headerSize, maxSize, &value);
	if (leb128Size != nullptr) *leb128Size = _leb128Size;
	if (value > maxSize - _leb128Size) {
		LOG_WARNING(
			"[AV1]The value of the obu size field is greater than the maximum size of the obu body. "
			"(%zu > %zu)", value, maxSize - _leb128Size);
		return maxSize - _leb128Size;
	}
	return value;
}
