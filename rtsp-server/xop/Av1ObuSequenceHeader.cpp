#include "Av1ObuSequenceHeader.h"
#include "Av1GetBitContext.h"

#define FF_PROFILE_AV1_MAIN 0
#define FF_PROFILE_AV1_HIGH 1
#define FF_PROFILE_AV1_PROFESSIONAL 2

using namespace xop;
using namespace std;

uint8_t Av1ObuSequenceHeader::GetProfile() const
{
	return profile_;
}

uint8_t Av1ObuSequenceHeader::GetLevel() const
{
	return level_;
}

uint8_t Av1ObuSequenceHeader::GetTier() const
{
	return tier_;
}

uint8_t Av1ObuSequenceHeader::GetBitdepth() const
{
	return bitdepth_;
}

uint8_t Av1ObuSequenceHeader::GetMonochrome() const
{
	return monochrome_;
}

uint8_t Av1ObuSequenceHeader::GetChromaSubsamplingX() const
{
	return chroma_subsampling_x_;
}

uint8_t Av1ObuSequenceHeader::GetChromaSubsamplingY() const
{
	return chroma_subsampling_y_;
}

uint8_t Av1ObuSequenceHeader::GetChromaSamplePosition() const
{
	return chroma_sample_position_;
}

uint8_t Av1ObuSequenceHeader::GetColorDescriptionPresentFlag() const
{
	return color_description_present_flag_;
}

uint8_t Av1ObuSequenceHeader::GetColorPrimaries() const
{
	return color_primaries_;
}

uint8_t Av1ObuSequenceHeader::GetTransferCharacteristics() const
{
	return transfer_characteristics_;
}

uint8_t Av1ObuSequenceHeader::GetMatrixCoefficients() const
{
	return matrix_coefficients_;
}

uint8_t Av1ObuSequenceHeader::GetColorRange() const
{
	return color_range_;
}

Av1ObuSequenceHeader::Av1ObuSequenceHeader(std::vector<uint8_t> obuBody)
{
	Av1GetBitContext gb(obuBody);
	int reduced_still_picture_header;
	int frame_width_bits_minus_1, frame_height_bits_minus_1;

	profile_ = gb.GetBits(3);

	gb.SkipBits(1); // still_picture
	reduced_still_picture_header = gb.GetBits(1);

	if (reduced_still_picture_header) {
		level_ = gb.GetBits(5);
		tier_ = 0;
	} else {
		int initial_display_delay_present_flag,
			operating_points_cnt_minus_1;
		int decoder_model_info_present_flag,
			buffer_delay_length_minus_1;

		if (gb.GetBits(1)) {          // timing_info_present_flag
			gb.SkipBits(32);      // num_units_in_display_tick
			gb.SkipBits(32);      // time_scale

			if (gb.GetBits(1))    // equal_picture_interval
				gb.Uvlc();    // num_ticks_per_picture_minus_1

			decoder_model_info_present_flag = gb.GetBits(1);
			if (decoder_model_info_present_flag) {
				buffer_delay_length_minus_1 = gb.GetBits(5);
				gb.SkipBits(32);
				gb.SkipBits(10);
			}
		} else
			decoder_model_info_present_flag = 0;

		initial_display_delay_present_flag = gb.GetBits(1);

		operating_points_cnt_minus_1 = gb.GetBits(5);
		for (int i = 0; i <= operating_points_cnt_minus_1; i++) {
			int seq_level_idx, seq_tier;

			gb.SkipBits(12);
			seq_level_idx = gb.GetBits(5);

			if (seq_level_idx > 7)
				seq_tier = gb.GetBits(1);
			else
				seq_tier = 0;

			if (decoder_model_info_present_flag) {
				if (gb.GetBits(1)) {
					gb.SkipBits(buffer_delay_length_minus_1 + 1);
					gb.SkipBits(buffer_delay_length_minus_1 + 1);
					gb.SkipBits(1);
				}
			}

			if (initial_display_delay_present_flag) {
				if (gb.GetBits(1))
					gb.SkipBits(4);
			}

			if (i == 0) {
				level_ = seq_level_idx;
				tier_ = seq_tier;
			}
		}
	}

	frame_width_bits_minus_1 = gb.GetBits(4);
	frame_height_bits_minus_1 = gb.GetBits(4);

	gb.SkipBits(frame_width_bits_minus_1 + 1);  // max_frame_width_minus_1
	gb.SkipBits(frame_height_bits_minus_1 + 1); // max_frame_height_minus_1

	if (!reduced_still_picture_header) {
		if (gb.GetBits(1))      // frame_id_numbers_present_flag
			gb.SkipBits(7);
	}

	gb.SkipBits(3); // use_128x128_superblock (1), enable_filter_intra (1), enable_intra_edge_filter (1)

	if (!reduced_still_picture_header) {
		int enable_order_hint, seq_force_screen_content_tools;

		gb.SkipBits(4);

		enable_order_hint = gb.GetBits(1);
		if (enable_order_hint)
			gb.SkipBits(2);

		if (gb.GetBits(1)) // seq_choose_screen_content_tools
			seq_force_screen_content_tools = 2;
		else
			seq_force_screen_content_tools = gb.GetBits(1);

		if (seq_force_screen_content_tools) {
			if (!gb.GetBits(1))     // seq_choose_integer_mv
				gb.SkipBits(1); // seq_force_integer_mv
		}

		if (enable_order_hint)
			gb.SkipBits(3);        // order_hint_bits_minus_1
	}

	gb.SkipBits(3);

	ParseColorConfig(&gb);

	gb.SkipBits(1);                        // film_grain_params_present

	/*if (gb.GetBitsLeft()) return -1; */
}

void Av1ObuSequenceHeader::ParseColorConfig(Av1GetBitContext *gb)
{
	int twelve_bit = 0;
	int high_bitdepth = gb->GetBits(1);
	if (profile_ == FF_PROFILE_AV1_PROFESSIONAL && high_bitdepth)
		twelve_bit = gb->GetBits(1);

	bitdepth_ = 8 + (high_bitdepth * 2) + (twelve_bit * 2);

	if (profile_ == FF_PROFILE_AV1_HIGH)
		monochrome_ = 0;
	else
		monochrome_ = gb->GetBits(1);

	color_description_present_flag_ = gb->GetBits(1);
	if (color_description_present_flag_) {
		color_primaries_ = gb->GetBits(8);
		transfer_characteristics_ = gb->GetBits(8);
		matrix_coefficients_ = gb->GetBits(8);
	} else {
		color_primaries_ = 2;
		transfer_characteristics_ = 2;
		matrix_coefficients_ = 2;
	}

	if (monochrome_) {
		color_range_ = gb->GetBits(1);
		chroma_subsampling_x_ = 1;
		chroma_subsampling_y_ = 1;
		chroma_sample_position_ = 0;
		return;
	} else if (color_primaries_ == 1 &&
		   transfer_characteristics_ == 13 &&
		   matrix_coefficients_ == 0) {
		chroma_subsampling_x_ = 0;
		chroma_subsampling_y_ = 0;
	} else {
		color_range_ = gb->GetBits(1);

		if (profile_ == FF_PROFILE_AV1_MAIN) {
			chroma_subsampling_x_ = 1;
			chroma_subsampling_y_ = 1;
		} else if (profile_ == FF_PROFILE_AV1_HIGH) {
			chroma_subsampling_x_ = 0;
			chroma_subsampling_y_ = 0;
		} else {
			if (twelve_bit) {
				chroma_subsampling_x_ =
					gb->GetBits(1);
				if (chroma_subsampling_x_)
					chroma_subsampling_y_ =
						gb->GetBits(1);
				else
					chroma_subsampling_y_ = 0;
			} else {
				chroma_subsampling_x_ = 1;
				chroma_subsampling_y_ = 0;
			}
		}
		if (chroma_subsampling_x_ &&
		    chroma_subsampling_y_)
			chroma_sample_position_ = gb->GetBits(2);
	}

	gb->SkipBits(1); // separate_uv_delta_q
}