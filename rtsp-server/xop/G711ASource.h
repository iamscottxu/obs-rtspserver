// PHZ
// 2018-5-16

#ifndef XOP_G711A_SOURCE_H
#define XOP_G711A_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"

namespace xop {

class G711ASource : public MediaSource {
public:
	static G711ASource *CreateNew();
	~G711ASource() override;

	uint32_t GetSampleRate() const { return samplerate_; }

	uint32_t GetChannels() const { return channels_; }

	std::string GetMediaDescription(uint16_t port = 0) override;

	std::string GetAttribute() override;

	bool HandleFrame(MediaChannelId channel_id, AVFrame frame) override;

	static uint32_t GetTimestamp();

private:
	G711ASource();

	uint32_t samplerate_ = 8000;
	uint32_t channels_ = 1;
};

}

#endif
