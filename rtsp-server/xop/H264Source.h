// PHZ
// 2018-5-16

#ifndef XOP_H264_SOURCE_H
#define XOP_H264_SOURCE_H

#include "MediaSource.h"
#include "rtp.h"
#include <vector>

namespace xop
{ 

class H264Source : public MediaSource
{
public:
	static H264Source *CreateNew(uint32_t framerate = 25);

	static H264Source *CreateNew(const std::vector<uint8_t> sps,
				     const std::vector<uint8_t> pps,
				     uint32_t framerate = 25);
	~H264Source();

	void SetFramerate(uint32_t framerate)
	{ framerate_ = framerate; }

	uint32_t GetFramerate() const 
	{ return framerate_; }

	virtual std::string GetMediaDescription(uint16_t port); 

	virtual std::string GetAttribute(); 

	bool HandleFrame(MediaChannelId channel_id, AVFrame frame);

	static uint32_t GetTimestamp();
	
private:
	H264Source(const std::vector<uint8_t> sps,
		   const std::vector<uint8_t> pps,
		   uint32_t framerate);

	static std::string Base64Encode(const void *input, size_t size);

	uint32_t framerate_ = 25;

	//uint32_t profileLevelId_;

	std::vector<uint8_t> sps_;

	std::vector<uint8_t> pps_;
};
	
}

#endif



