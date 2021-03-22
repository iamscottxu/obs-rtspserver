// PHZ
// 2018-5-16

#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "H264Source.h"
#include <memory>
#include <vector>
#include <cstdio>
#include <chrono>
#if defined(WIN32) || defined(_WIN32)

#else
#include <sys/time.h>
#endif
extern "C" {
#include "libb64/include/b64/cencode.h"
}

using namespace xop;
using namespace std;

H264Source::H264Source(const vector<uint8_t> sps, const vector<uint8_t> pps,
		       uint32_t framerate)
	: framerate_(framerate),
	  sps_(sps), pps_(pps)
{
    payload_    = 96; 
    media_type_ = H264;
    clock_rate_ = 90000;
}

H264Source *H264Source::CreateNew(uint32_t framerate)
{
	return new H264Source(vector<uint8_t>(), vector<uint8_t>(), framerate);
}

H264Source *H264Source::CreateNew(const vector<uint8_t> sps,
				  const vector<uint8_t> pps, uint32_t framerate)
{
	return new H264Source(sps, pps, framerate);
}

H264Source::~H264Source()
{

}

string H264Source::GetMediaDescription(uint16_t port)
{
	char buf[100] = {0};
	sprintf(buf, "m=video %hu RTP/AVP 96", port); // \r\nb=AS:2000

	return string(buf);
}

string H264Source::GetAttribute()
{
	auto sdp = string("a=rtpmap:96 H264/90000\r\n");

	if (!sps_.empty() && !pps_.empty()) {
		char const *fmtp = "a=fmtp:96 packetization-mode=1;"
				   "profile-level-id=%06X;"
				   "sprop-parameter-sets=%s,%s";

		const auto pps_base64 = Base64Encode(pps_.data(), pps_.size());
		const auto sps_base64 = Base64Encode(sps_.data(), sps_.size());

		const uint32_t profile_level_id = (sps_.at(1) << 16) |
					    (sps_.at(2) << 8) | sps_.at(3);

		const size_t buf_size = 1 + strlen(fmtp) + 6 +
				       sps_base64.length() +
				  pps_base64.length();
		auto buf = vector<char>(buf_size);

		sprintf(buf.data(), fmtp, profile_level_id, sps_base64.c_str(),
			pps_base64.c_str());

		sdp.append(buf.data());
	}

        return sdp;
}

bool H264Source::HandleFrame(MediaChannelId channel_id, AVFrame frame)
{
    uint8_t* frame_buf  = frame.buffer.get();
    uint32_t frame_size = frame.size;

	if (frame.timestamp == 0) {
		frame.timestamp = GetTimestamp();
	}    

    if (frame_size <= MAX_RTP_PAYLOAD_SIZE) {
        RtpPacket rtp_pkt;
		rtp_pkt.type = frame.type;
		rtp_pkt.timestamp = frame.timestamp;
		rtp_pkt.size = frame_size + 4 + RTP_HEADER_SIZE;
		rtp_pkt.last = 1;
        memcpy(rtp_pkt.data.get()+4+RTP_HEADER_SIZE, frame_buf, frame_size); 

        if (send_frame_callback_) {
			if (!send_frame_callback_(channel_id, rtp_pkt)) {
				return false;
			}               
        }
    }
    else {
        char FU_A[2] = {0};

        FU_A[0] = (frame_buf[0] & 0xE0) | 28;
        FU_A[1] = 0x80 | (frame_buf[0] & 0x1f);

        frame_buf  += 1;
        frame_size -= 1;

        while (frame_size + 2 > MAX_RTP_PAYLOAD_SIZE) {
            RtpPacket rtp_pkt;
            rtp_pkt.type = frame.type;
            rtp_pkt.timestamp = frame.timestamp;
            rtp_pkt.size = 4 + RTP_HEADER_SIZE + MAX_RTP_PAYLOAD_SIZE;
            rtp_pkt.last = 0;

            rtp_pkt.data.get()[RTP_HEADER_SIZE+4] = FU_A[0];
            rtp_pkt.data.get()[RTP_HEADER_SIZE+5] = FU_A[1];
            memcpy(rtp_pkt.data.get()+4+RTP_HEADER_SIZE+2, frame_buf, MAX_RTP_PAYLOAD_SIZE-2);

            if (send_frame_callback_) {
                if (!send_frame_callback_(channel_id, rtp_pkt))
                    return false;
            }

            frame_buf  += MAX_RTP_PAYLOAD_SIZE - 2;
            frame_size -= MAX_RTP_PAYLOAD_SIZE - 2;

            FU_A[1] &= ~0x80;
        }

        {
            RtpPacket rtp_pkt;
            rtp_pkt.type = frame.type;
            rtp_pkt.timestamp = frame.timestamp;
            rtp_pkt.size = 4 + RTP_HEADER_SIZE + 2 + frame_size;
            rtp_pkt.last = 1;

            FU_A[1] |= 0x40;
            rtp_pkt.data.get()[RTP_HEADER_SIZE+4] = FU_A[0];
            rtp_pkt.data.get()[RTP_HEADER_SIZE+5] = FU_A[1];
            memcpy(rtp_pkt.data.get()+4+RTP_HEADER_SIZE+2, frame_buf, frame_size);

            if (send_frame_callback_) {
				if (!send_frame_callback_(channel_id, rtp_pkt)) {
					return false;
				}              
            }
        }
    }

    return true;
}

uint32_t H264Source::GetTimestamp()
{
/* #if defined(__linux) || defined(__linux__)
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	uint32_t ts = ((tv.tv_sec*1000)+((tv.tv_usec+500)/1000))*90; // 90: _clockRate/1000;
	return ts;
#else  */
	auto time_point = chrono::time_point_cast<chrono::microseconds>(chrono::steady_clock::now());
	return (uint32_t)((time_point.time_since_epoch().count() + 500) / 1000 * 90 );
//#endif
}

std::string H264Source::Base64Encode(const void *input, size_t size)
{
	std::vector<char> buffer(size / 3 * 4 + (size % 3 > 0 ? 4 : 0) + 1);
	base64_encodestate b64encoder;
	base64_init_encodestate(&b64encoder);

	auto length = base64_encode_block(
		reinterpret_cast<const char *>(input),
				      int(size), buffer.data(), &b64encoder);
	base64_encode_blockend(buffer.data() + length, &b64encoder);

	return std::string(buffer.cbegin(), buffer.cend() - 1);
}

