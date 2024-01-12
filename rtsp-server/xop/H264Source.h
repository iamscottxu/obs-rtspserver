// PHZ
// 2018-5-16

#ifndef XOP_H264_SOURCE_H
#define XOP_H264_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"
#include "H26xNalUnit.h"

namespace xop {

class H264Source : public MediaSource {
public:
	static H264Source *CreateNew(std::vector<uint8_t> extraData,
				     uint32_t framerate = 25);

	static H264Source *CreateNew(std::vector<uint8_t> sps,
				     std::vector<uint8_t> pps,
				     uint32_t framerate = 25);
	~H264Source() override;

	void SetFramerate(const uint32_t framerate) { framerate_ = framerate; }

	uint32_t GetFramerate() const { return framerate_; }

	std::string GetMediaDescription(uint16_t port) override;

	std::string GetAttribute() override;

	bool HandleFrame(MediaChannelId channel_id, AVFrame frame) override;

	static uint32_t GetTimestamp();

private:
	H264Source(std::vector<uint8_t> sps, std::vector<uint8_t> pps,
		   uint32_t framerate);

	static FrameType GetRtpFrameType(std::shared_ptr<H26xNalUnit> nalUnit);

	uint32_t framerate_ = 25;

	//uint32_t profileLevelId_;

	std::vector<uint8_t> sps_;

	std::vector<uint8_t> pps_;
};

}

#endif
