// PHZ
// 2018-5-16

#ifndef XOP_MEDIA_H
#define XOP_MEDIA_H

#include <memory>

namespace xop
{

/* RTSP服务支持的媒体类型 */
enum MediaType
{
	//PCMU = 0,	 
	PCMA = 8,
	H264 = 96,
	AAC  = 37,
	H265 = 265,   
	NONE
};	

enum FrameType
{
	VIDEO_FRAME_I = 0x01,	  
	VIDEO_FRAME_P = 0x02,
	VIDEO_FRAME_B = 0x03,    
	AUDIO_FRAME   = 0x11,   
};

struct AVFrame
{	
	AVFrame(uint32_t size = 0)
		:buffer(new uint8_t[size])
	{
		this->size = size;
		type = 0;
		timestamp = 0;
	}

	std::shared_ptr<uint8_t> buffer; /* 帧数据 */
	uint32_t size;				     /* 帧大小 */
	uint8_t  type;				     /* 帧类型 */	
	uint32_t timestamp;		  	     /* 时间戳 */
};

enum MediaChannelId
{
	channel_0,
	channel_1,
        channel_2,
        channel_3,
        channel_4,
        channel_5,
        channel_6,
        channel_7,
        channel_8,
        channel_9
};

typedef uint32_t MediaSessionId;

}

#endif

