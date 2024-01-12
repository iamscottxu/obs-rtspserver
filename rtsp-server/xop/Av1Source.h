#pragma once

#include "MediaSource.h"

namespace xop {

class Av1ObuSequenceHeader;
class Av1Tu;

class Av1Source : public MediaSource {
public:
	static Av1Source *
	CreateNew(std::vector<uint8_t> extraData,
		  uint32_t framerate = 25);

	static Av1Source *
	CreateNew(std::shared_ptr<Av1ObuSequenceHeader> sequenceHeader, uint32_t framerate = 25);

	~Av1Source() override;

	void SetFramerate(const uint32_t framerate) { framerate_ = framerate; }

	uint32_t GetFramerate() const { return framerate_; }

	std::string GetMediaDescription(uint16_t port = 0) override;

	std::string GetAttribute() override;

	bool HandleFrame(MediaChannelId channelId, AVFrame frame) override;

	static uint32_t GetTimestamp();

private:
	Av1Source(std::shared_ptr<Av1ObuSequenceHeader> sequenceHeader, const uint32_t framerate);

	static FrameType GetRtpFrameType(const Av1Tu &obu);

	uint32_t framerate_;

	std::shared_ptr<Av1ObuSequenceHeader> sequence_header_;
};

}

