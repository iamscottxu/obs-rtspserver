// PHZ
// 2018-5-16

#ifndef XOP_MEDIA_H
#define XOP_MEDIA_H

#include <memory>

namespace xop {

/* RTSP服务支持的媒体类型 */
enum class MediaType {
	//PCMU = 0,
	PCMA = 8,
	H264 = 96,
	AAC = 37,
	H265 = 265,
	NONE
};

enum class FrameType : uint8_t {
	VIDEO_FRAME_I = 0x01,
	VIDEO_FRAME_P = 0x02,
	VIDEO_FRAME_B = 0x03,
	AUDIO_FRAME = 0x11,
	NONE = 0x00
};

struct AVFrame {
	explicit AVFrame(const size_t size = 0)
		: buffer(new uint8_t[size], std::default_delete<uint8_t[]>()),
		  size(size),
		  type(FrameType::NONE),
		  timestamp(0)
	{
	}

	std::shared_ptr<uint8_t> buffer; /* 帧数据 */
	size_t size;                     /* 帧大小 */
	FrameType type;                  /* 帧类型 */
	uint32_t timestamp;              /* 时间戳 */
};

enum class MediaChannelId : uint8_t {
	channel_0 = 0,
	channel_1 = 1,
	channel_2 = 2,
	channel_3 = 3,
	channel_4 = 4,
	channel_5 = 5,
	channel_6 = 6,
	channel_7 = 7,
	channel_8 = 8,
	channel_9 = 9
};

typedef uint32_t MediaSessionId;

}

#endif
