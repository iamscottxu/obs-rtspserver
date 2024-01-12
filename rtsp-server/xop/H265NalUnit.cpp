#include "H265NalUnit.h"

using namespace xop;

H265NalUnit::H265NalUnit(const uint8_t *data, size_t dataSize) : H26xNalUnit(data, dataSize)
{
}

uint8_t H265NalUnit::GetType()
{
	uint8_t *data;
	if (GetHeader(&data) == H265_NALU_HEADER_SIZE) {
		return (data[0] & 0x7e) >> 1;
	}
	return 0;
}

uint8_t H265NalUnit::GetLayerId()
{
	uint8_t *data;
	if (GetHeader(&data) == H265_NALU_HEADER_SIZE) {
		return (data[0] << 5 & 0x20) | (data[1] >> 3 & 0x1f);
	}
	return 0;
}

uint8_t H265NalUnit::GetTemporalId()
{
	uint8_t *data;
	if (GetHeader(&data) == H265_NALU_HEADER_SIZE) {
		return data[1] & 0x07;
	}
	return 0;
}

H265NalType H265NalUnit::GetH265Type()
{
	return static_cast<H265NalType>(GetType());
}

size_t H265NalUnit::GetHeader(uint8_t **data)
{
	if (GetData(data) >= H265_NALU_HEADER_SIZE)
		return H265_NALU_HEADER_SIZE;
	return 0;
}

size_t H265NalUnit::CopyHeader(uint8_t *start, size_t size)
{
	if (size > H265_NALU_HEADER_SIZE) {
		size = H265_NALU_HEADER_SIZE;
	}
	return CopyData(start, size);
}

size_t H265NalUnit::GetHeaderSize()
{
	const auto size = GetSize();
	if (size > H265_NALU_HEADER_SIZE)
		return H265_NALU_HEADER_SIZE;
	return 0;
}

size_t H265NalUnit::GetBody(uint8_t **data)
{
	const auto size = GetData(data);
	if (size > H265_NALU_HEADER_SIZE) {
		*data += H265_NALU_HEADER_SIZE;
		return size - H265_NALU_HEADER_SIZE;
	}
	return 0;
}

size_t H265NalUnit::CopyBody(uint8_t *start, size_t size, size_t skip)
{
	skip += H265_NALU_HEADER_SIZE;
	return CopyData(start, size, skip);
}

size_t H265NalUnit::GetBodySize()
{
	const auto size = GetSize();
	if (size > H265_NALU_HEADER_SIZE)
		return size - H265_NALU_HEADER_SIZE;
	return 0;
}

bool H265NalUnit::IsIdrFrame()
{
	const auto type = GetH265Type();
	return type == H265NalType::H265_NAL_IDR_N_LP || type == H265NalType::H265_NAL_IDR_W_RADL;
}

bool H265NalUnit::IsFrame()
{
	const auto type = GetH265Type();
	return type <= H265NalType::H265_NAL_CRA_NUT;
}

H26xNalUnit * H265NalUnit::GetNalUnit(const uint8_t *data, size_t dataSize)
{
	return new H265NalUnit(data, dataSize);
}

