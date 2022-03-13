// PHZ
// 2021-8-26

#ifndef XOP_VP8_SOURCE_H
#define XOP_VP8_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"

namespace xop {

class VP8Source : public MediaSource {
public:
	static VP8Source *CreateNew(uint32_t framerate = 25);
	~VP8Source() override;

	void Setframerate(const uint32_t framerate) { framerate_ = framerate; }

	uint32_t GetFramerate() const { return framerate_; }

	std::string GetMediaDescription(uint16_t port = 0) override;

	std::string GetAttribute() override;

	bool HandleFrame(MediaChannelId channelId, AVFrame frame) override;

	static uint32_t GetTimestamp();

private:
	explicit VP8Source(uint32_t framerate);

	uint32_t framerate_ = 25;
};

}
#endif
