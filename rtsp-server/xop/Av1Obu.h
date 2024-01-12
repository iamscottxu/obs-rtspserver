#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Av1ObuSequenceHeader.h"

namespace xop {

enum class Av1ObuType : uint8_t {
	AV1_OBU_RESERVED0 = 0,
	AV1_OBU_SEQUENCE_HEADER = 1,
	AV1_OBU_TEMPORAL_DELIMITER = 2,
	AV1_OBU_FRAME_HEADER = 3,
	AV1_OBU_TILE_GROUP = 4,
	AV1_OBU_METADATA = 5,
	AV1_OBU_FRAME = 6,
	AV1_OBU_REDUNDANT_FRAME_HEADER = 7,
	AV1_OBU_TILE_LIST = 8,
	AV1_OBU_RESERVED9 = 9,
	AV1_OBU_RESERVED10 = 10,
	AV1_OBU_RESERVED11 = 11,
	AV1_OBU_RESERVED12 = 12,
	AV1_OBU_RESERVED13 = 13,
	AV1_OBU_RESERVED14 = 14,
	AV1_OBU_PADDING = 15,
};

class Av1Obu {
public:
	Av1Obu() = delete;

	size_t GetData(uint8_t **data);
	std::vector<uint8_t> GetData();
	size_t CopyData(uint8_t *start, size_t size, size_t skip = 0);
	size_t GetSize();

	std::vector<uint8_t> GetNotHasSizeFieldData();
	size_t CopyNotHasSizeFieldData(uint8_t *start, size_t size, size_t skip = 0);
	size_t GetNotHasSizeFieldSize();

	size_t GetHeader(uint8_t **data);
	std::vector<uint8_t> GetHeader();
	size_t CopyHeader(uint8_t *start, size_t size, size_t skip = 0);
	size_t GetHeaderSize();

	size_t GetBody(uint8_t **data);
	std::vector<uint8_t> GetBody();
	size_t CopyBody(uint8_t *start, size_t size, size_t skip = 0);
	size_t GetBodySize();

	/*Basic Header*/
	Av1ObuType GetType();
	bool HasExtensionHeader();
	bool HasSizeField();

	/*Extension Header*/
	uint8_t GetTemporalId();
	uint8_t GetSpatialId();

	std::shared_ptr<Av1ObuSequenceHeader> GetSequenceHeader();

	static Av1Obu *GetAv1ObuUnit(const uint8_t *data, size_t dataSize);

private:
	Av1Obu(const uint8_t *data, size_t dataSize);
	bool IsForbiddenBitZero();
	size_t GetBodySizeWithLeb128Size(size_t *leb128Size);
	const uint8_t *data_;
	size_t data_size_;
};

}
