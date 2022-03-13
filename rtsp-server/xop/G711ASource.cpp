// PHZ
// 2018-5-16

#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include "G711ASource.h"
#include <cstdio>
#include <chrono>
#if defined(WIN32) || defined(_WIN32)

#else
#include <sys/time.h>
#endif

using namespace xop;
using namespace std;

G711ASource::G711ASource()
{
	payload_ = 8;
	media_type_ = MediaType::PCMA;
	clock_rate_ = 8000;
}

G711ASource *G711ASource::CreateNew()
{
	return new G711ASource();
}

G711ASource::~G711ASource() = default;

string G711ASource::GetMediaDescription(const uint16_t port)
{
	char buf[100] = {0};
	sprintf(buf, "m=audio %hu RTP/AVP 8", port);

	return buf;
}

string G711ASource::GetAttribute()
{
	return "a=rtpmap:8 PCMA/8000/1";
}

bool G711ASource::HandleFrame(const MediaChannelId channel_id,
			      const AVFrame frame)
{
	if (frame.size > MAX_RTP_PAYLOAD_SIZE) {
		return false;
	}

	const uint8_t *frame_buf = frame.buffer.get();
	const size_t frame_size = frame.size;

	RtpPacket rtp_pkt;
	rtp_pkt.type = frame.type;
	rtp_pkt.timestamp = frame.timestamp;
	rtp_pkt.size = frame_size + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE;
	rtp_pkt.last = 1;

	memcpy(rtp_pkt.data.get() + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE,
	       frame_buf, frame_size);

	if (send_frame_callback_) {
		return send_frame_callback_(channel_id, rtp_pkt); //TODO
	}

	return true;
}

uint32_t G711ASource::GetTimestamp()
{
	const auto time_point = chrono::time_point_cast<chrono::microseconds>(
		chrono::steady_clock::now());
	return static_cast<uint32_t>(
		(time_point.time_since_epoch().count() + 500) / 1000 * 8);
}
