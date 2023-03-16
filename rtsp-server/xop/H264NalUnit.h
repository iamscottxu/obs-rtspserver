#ifndef XOP_H264_NALUNIT_H
#define XOP_H264_NALUNIT_H

#include <cstddef>
#include "NalUnit.h"

#define H264_NALU_HEADER_SIZE 1

namespace xop {

enum class H264NalType: uint8_t {
	H264_NAL_UNKNOWN = 0,
	H264_NAL_SLICE = 1,
	H264_NAL_SLICE_DPA = 2,
	H264_NAL_SLICE_DPB = 3,
	H264_NAL_SLICE_DPC = 4,
	H264_NAL_SLICE_IDR = 5,
	H264_NAL_SEI = 6,
	H264_NAL_SPS = 7,
	H264_NAL_PPS = 8,
	H264_NAL_AUD = 9,
	H264_NAL_EOSEQ = 10,
	H264_NAL_EOSTREAM = 11,
	H264_NAL_FILLER = 12,
	H264_NAL_RSV13 = 13,
	H264_NAL_RSV14 = 14,
	H264_NAL_RSV15 = 15,
	H264_NAL_RSV16 = 16,
	H264_NAL_RSV17 = 17,
	H264_NAL_RSV18 = 18,
	H264_NAL_RSV19 = 19,
	H264_NAL_RSV20 = 20,
	H264_NAL_RSV21 = 21,
	H264_NAL_RSV22 = 22,
	H264_NAL_RSV23 = 23,
	H264_NAL_UNSPEC24 = 24,
	H264_NAL_UNSPEC25 = 25,
	H264_NAL_UNSPEC26 = 26,
	H264_NAL_UNSPEC27 = 27,
	H264_NAL_UNSPEC28 = 28,
	H264_NAL_UNSPEC29 = 29,
	H264_NAL_UNSPEC30 = 30,
	H264_NAL_UNSPEC31 = 31
};

class H264NalUnit : public NalUnit {
public:
	uint8_t GetType() override;
	uint8_t GetRefIdc();
	H264NalType GetH264Type();
	size_t GetHeader(uint8_t **data) override;
	size_t CopyHeader(uint8_t *start, size_t size) override;
	size_t GetHeaderSize() override;
	size_t GetBody(uint8_t **data) override;
	size_t CopyBody(uint8_t *start, size_t size, size_t skip = 0) override;
	size_t GetBodySize() override;
	bool IsIdrFrame() override;
	bool IsFrame() override;
	static NalUnit *GetNalUnit(const uint8_t *data, size_t dataSize);

private:
	H264NalUnit(const uint8_t *data, size_t dataSize);
};

}

#endif
