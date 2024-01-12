#pragma once

#include <cstdint>
#include <vector>

namespace xop {
class Av1GetBitContext;

class Av1ObuSequenceHeader {
	friend class Av1Obu;

public:
	uint8_t GetProfile() const;
	uint8_t GetLevel() const;
	uint8_t GetTier() const;
	uint8_t GetBitdepth() const;
	uint8_t GetMonochrome() const;
	uint8_t GetChromaSubsamplingX() const;
	uint8_t GetChromaSubsamplingY() const;
	uint8_t GetChromaSamplePosition() const;
	uint8_t GetColorDescriptionPresentFlag() const;
	uint8_t GetColorPrimaries() const;
	uint8_t GetTransferCharacteristics() const;
	uint8_t GetMatrixCoefficients() const;
	uint8_t GetColorRange() const;

private:
	explicit Av1ObuSequenceHeader(std::vector<uint8_t> obuBody);
	void ParseColorConfig(Av1GetBitContext *gb);

	uint8_t profile_;
	uint8_t level_;
	uint8_t tier_;
	uint8_t bitdepth_{};
	uint8_t monochrome_{};
	uint8_t chroma_subsampling_x_{};
	uint8_t chroma_subsampling_y_{};
	uint8_t chroma_sample_position_{};
	uint8_t color_description_present_flag_{};
	uint8_t color_primaries_{};
	uint8_t transfer_characteristics_{};
	uint8_t matrix_coefficients_{};
	uint8_t color_range_{};
};
}