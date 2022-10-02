#include "H264NalUnit.h"

using namespace xop;

H264NalUnit::H264NalUnit(const uint8_t *data, size_t dataSize)
	: NalUnit(data, dataSize)
{
}

uint8_t H264NalUnit::GetType()
{
	uint8_t *data;
	if (GetHeader(&data) == H264_NALU_HEADER_SIZE) {
		return data[0] & 0x1f;
	}
	return 0;
}

H264NalType H264NalUnit::GetH264Type()
{
	return static_cast<H264NalType>(GetType());
}

size_t H264NalUnit::GetHeader(uint8_t **data)
{
	if (GetData(data) >= H264_NALU_HEADER_SIZE)
		return H264_NALU_HEADER_SIZE;
	return 0;
}

size_t H264NalUnit::CopyHeader(uint8_t *start, size_t size)
{
	if (size > H264_NALU_HEADER_SIZE) {
		size = H264_NALU_HEADER_SIZE;
	}
	return CopyData(start, size);
}

size_t H264NalUnit::GetHeaderSize()
{
	const auto size = GetSize();
	if (size > H264_NALU_HEADER_SIZE)
		return H264_NALU_HEADER_SIZE;
	return 0;
}

size_t H264NalUnit::GetBody(uint8_t **data)
{
	const auto size = GetData(data);
	if (size > H264_NALU_HEADER_SIZE) {
		*data += H264_NALU_HEADER_SIZE;
		return size - H264_NALU_HEADER_SIZE;
	}
	return 0;
}

size_t H264NalUnit::CopyBody(uint8_t *start, size_t size, size_t skip)
{
	skip += H264_NALU_HEADER_SIZE;
	return CopyData(start, size, skip);
}

size_t H264NalUnit::GetBodySize()
{
	const auto size = GetSize();
	if (size > H264_NALU_HEADER_SIZE)
		return size - H264_NALU_HEADER_SIZE;
	return 0;
}

bool H264NalUnit::IsIdrFrame()
{
	const auto type = GetH264Type();
	return type == H264NalType::H264_NAL_SLICE_IDR;
}

bool H264NalUnit::IsFrame()
{
	const auto type = GetH264Type();
	return type >= H264NalType::H264_NAL_SLICE && type <= H264NalType::H264_NAL_SLICE_IDR;
}

NalUnit * H264NalUnit::GetNalUnit(const uint8_t *data, size_t dataSize)
{
	return new H264NalUnit(data, dataSize);
}
