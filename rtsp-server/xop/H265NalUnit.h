#ifndef XOP_H265_NALUNIT_H
#define XOP_H265_NALUNIT_H

#include <cstddef>
#include "NalUnit.h"

#define H265_NALU_HEADER_SIZE 2

namespace xop {

enum class H265NalType: uint8_t {
	H265_NAL_TRAIL_N = 0,
	H265_NAL_TRAIL_R = 1,
	H265_NAL_TSA_N = 2,
	H265_NAL_TSA_R = 3,
	H265_NAL_STSA_N = 4,
	H265_NAL_STSA_R = 5,
	H265_NAL_RADL_N = 6,
	H265_NAL_RADL_R = 7,
	H265_NAL_RASL_N = 8,
	H265_NAL_RASL_R = 9,
	H265_NAL_VCL_N10 = 10,
	H265_NAL_VCL_R11 = 11,
	H265_NAL_VCL_N12 = 12,
	H265_NAL_VCL_R13 = 13,
	H265_NAL_VCL_N14 = 14,
	H265_NAL_VCL_R15 = 15,
	H265_NAL_BLA_W_LP = 16,
	H265_NAL_BLA_W_RADL = 17,
	H265_NAL_BLA_N_LP = 18,
	H265_NAL_IDR_W_RADL = 19,
	H265_NAL_IDR_N_LP = 20,
	H265_NAL_CRA_NUT = 21,
	H265_NAL_RSV_IRAP_VCL22 = 22,
	H265_NAL_RSV_IRAP_VCL23 = 23,
	H265_NAL_RSV_VCL24 = 24,
	H265_NAL_RSV_VCL25 = 25,
	H265_NAL_RSV_VCL26 = 26,
	H265_NAL_RSV_VCL27 = 27,
	H265_NAL_RSV_VCL28 = 28,
	H265_NAL_RSV_VCL29 = 29,
	H265_NAL_RSV_VCL30 = 30,
	H265_NAL_RSV_VCL31 = 31,
	H265_NAL_VPS = 32,
	H265_NAL_SPS = 33,
	H265_NAL_PPS = 34,
	H265_NAL_AUD = 35,
	H265_NAL_EOS_NUT = 36,
	H265_NAL_EOB_NUT = 37,
	H265_NAL_FD_NUT = 38,
	H265_NAL_SEI_PREFIX = 39,
	H265_NAL_SEI_SUFFIX = 40,
	H265_NAL_RSV_NVCL41 = 41,
	H265_NAL_RSV_NVCL42 = 42,
	H265_NAL_RSV_NVCL43 = 43,
	H265_NAL_RSV_NVCL44 = 44,
	H265_NAL_RSV_NVCL45 = 45,
	H265_NAL_RSV_NVCL46 = 46,
	H265_NAL_RSV_NVCL47 = 47,
	H265_NAL_UNSPEC48 = 48,
	H265_NAL_UNSPEC49 = 49,
	H265_NAL_UNSPEC50 = 50,
	H265_NAL_UNSPEC51 = 51,
	H265_NAL_UNSPEC52 = 52,
	H265_NAL_UNSPEC53 = 53,
	H265_NAL_UNSPEC54 = 54,
	H265_NAL_UNSPEC55 = 55,
	H265_NAL_UNSPEC56 = 56,
	H265_NAL_UNSPEC57 = 57,
	H265_NAL_UNSPEC58 = 58,
	H265_NAL_UNSPEC59 = 59,
	H265_NAL_UNSPEC60 = 60,
	H265_NAL_UNSPEC61 = 61,
	H265_NAL_UNSPEC62 = 62,
	H265_NAL_UNSPEC63 = 63,
};

class H265NalUnit : public NalUnit {
public:
	uint8_t GetType() override;
	uint8_t GetLayerId();
	uint8_t GetTemporalId();
	H265NalType GetH265Type();
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
	H265NalUnit(const uint8_t *data, size_t dataSize);

};

}

#endif
