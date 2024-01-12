// PHZ
// 2018-5-16

#ifndef XOP_H265_SOURCE_H
#define XOP_H265_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"
#include "H26xNalUnit.h"

namespace xop {

class H265Source : public MediaSource {
public:
	static H265Source *
	CreateNew(std::vector<uint8_t> extraData,
		  std::vector<uint8_t> sei = std::vector<uint8_t>(),
		  uint32_t framerate = 25);

	static H265Source *
	CreateNew(std::vector<uint8_t> vps, std::vector<uint8_t> sps,
		  std::vector<uint8_t> pps,
		  std::vector<uint8_t> sei = std::vector<uint8_t>(),
		  uint32_t framerate = 25);

	~H265Source() override;

	void SetFramerate(const uint32_t framerate) { framerate_ = framerate; }

	uint32_t GetFramerate() const { return framerate_; }

	std::string GetMediaDescription(uint16_t port = 0) override;

	std::string GetAttribute() override;

	bool HandleFrame(MediaChannelId channelId, AVFrame frame) override;

	static uint32_t GetTimestamp();

private:
	H265Source(std::vector<uint8_t> vps, std::vector<uint8_t> sps,
		   std::vector<uint8_t> pps, std::vector<uint8_t> sei,
		   uint32_t framerate);

	static FrameType GetRtpFrameType(std::shared_ptr<H26xNalUnit> nalUnit);

	uint32_t framerate_ = 25;

	std::vector<uint8_t> vps_;

	std::vector<uint8_t> sps_;

	std::vector<uint8_t> pps_;

	std::vector<uint8_t> sei_;
};

}

#endif
