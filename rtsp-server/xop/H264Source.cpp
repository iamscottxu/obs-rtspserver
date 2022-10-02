// PHZ
// 2018-5-16

#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "H264Source.h"
#include <memory>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstring>
#include <chrono>
#if defined(WIN32) || defined(_WIN32)

#else
#include <sys/time.h>
#endif

#include "Base64Encode.h"
#include "Nal.h"
#include "H264NalUnit.h"
#include "H265Source.h"

using namespace xop;
using namespace std;

H264Source::H264Source(vector<uint8_t> sps, vector<uint8_t> pps,
		       const uint32_t framerate)
	: framerate_(framerate), sps_(move(sps)), pps_(move(pps))
{
	payload_ = 96;
	media_type_ = MediaType::H264;
	clock_rate_ = 90000;
}

H264Source *H264Source::CreateNew(vector<uint8_t> extraData,
				  const uint32_t framerate)
{
	Nal<H264NalUnit> nal(extraData);
	vector<uint8_t> sps, pps;
	const auto sps_nal_unit = nal.GetNalUnitByType(
			   static_cast<uint8_t>(H264NalType::H264_NAL_SPS)),
		   pps_nal_unit = nal.GetNalUnitByType(
			   static_cast<uint8_t>(H264NalType::H264_NAL_PPS));
	if (sps_nal_unit != nullptr)
		sps = sps_nal_unit->GetData();
	if (pps_nal_unit != nullptr)
		pps = pps_nal_unit->GetData();

	return new H264Source(sps, pps, framerate);
}

H264Source *H264Source::CreateNew(vector<uint8_t> sps, vector<uint8_t> pps,
				  const uint32_t framerate)
{
	return new H264Source(move(sps), move(pps), framerate);
}

H264Source::~H264Source() = default;

string H264Source::GetMediaDescription(const uint16_t port)
{
	char buf[100] = {0};
	sprintf(buf, "m=video %hu RTP/AVP 96", port); // \r\nb=AS:2000
	return buf;
}

string H264Source::GetAttribute()
{
	auto sdp = string("a=rtpmap:96 H264/90000\r\n");

	if (!sps_.empty() && !pps_.empty()) {
		const auto fmtp = "a=fmtp:96 packetization-mode=1;"
				  "profile-level-id=%06X;"
				  "sprop-parameter-sets=%s,%s";

		const auto pps_base64 = Base64Encode(pps_.data(), pps_.size());
		const auto sps_base64 = Base64Encode(sps_.data(), sps_.size());

		const uint32_t profile_level_id =
			(sps_.at(1) << 16) | (sps_.at(2) << 8) | sps_.at(3);

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

bool H264Source::HandleFrame(const MediaChannelId channelId,
			     const AVFrame frame)
{
	RtpPacket rtp_packet;
	//rtpPacket.timestamp = frame.timestamp == 0 ? GetTimestamp() : frame.timestamp;
	rtp_packet.timestamp = frame.timestamp;
	const auto rtp_packet_data =
		rtp_packet.data.get() + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE;

	Nal<H264NalUnit> nal(frame.buffer.get(), frame.size);

	if (nal.GetCount() == 0)
		return false;

	size_t nal_index = 0;
	while (nal_index < nal.GetCount()) {
		size_t end_index = nal_index;
		size_t size_count = H264_NALU_HEADER_SIZE;
		while (size_count < MAX_RTP_PAYLOAD_SIZE &&
		       end_index < nal.GetCount()) {
			size_count += nal[end_index++]->GetSize() + 2;
		}
		end_index--;
		if (size_count > MAX_RTP_PAYLOAD_SIZE && end_index > nal_index)
			size_count -= nal[end_index--]->GetSize() + 2;
		if (end_index > nal_index) {
			//Aggregation Packets
			/*  0                   1                   2                   3
             *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * |                          RTP Header                           |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * |PayloadHdr (28)|         NALU 1 Size           |   NALU 1 HDR  |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * |                                                               |
             * |                         NALU 1 Data                           |
             * |                   . . .                                       |
             * |                                                               |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * |         NALU 2 Size           |   NALU 2 HDR  |               |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
             * |                                                               |
             * |                        NALU 2 Data                            |
             * |                   . . .                                       |
             * |                                                               |
             * |                                                               |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             */
			rtp_packet.size = RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE +
					  static_cast<uint16_t>(size_count);
			rtp_packet.last = 1;
			size_t skip = H264_NALU_HEADER_SIZE;
			auto all_frame_type = FrameType::NONE;
			for (; nal_index <= end_index; nal_index++) {
				const auto nal_unit = nal[nal_index];
				const auto frame_type =
					GetRtpFrameType(nal_unit);
				if (frame_type == FrameType::VIDEO_FRAME_IDR)
					all_frame_type =
						FrameType::VIDEO_FRAME_IDR;
				else if (all_frame_type == FrameType::NONE)
					all_frame_type = frame_type;

				const auto size = static_cast<uint16_t>(
					nal_unit->GetSize());
				rtp_packet_data[skip++] = size >> 8;
				rtp_packet_data[skip++] = size & 0xff;
				skip += nal_unit->CopyData(
					rtp_packet_data + skip,
					MAX_RTP_PAYLOAD_SIZE -
						H264_NALU_HEADER_SIZE - 2);
			}
			//PayloadHeader
			rtp_packet_data[0] = 0x58; //28;
			rtp_packet.type = all_frame_type;
			if (!send_frame_callback_(channelId, rtp_packet))
				return false;
		} else {
			//Single NAL Unit Packets
			/*  0                   1                   2                   3
             *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * |  PayloadHdr   |                                               |
             * +-+-+-+-+-+-+-+-+                                               |
             * |                                                               |
             * |                                                               |
             * |                  NAL unit payload data                        |
             * |                                                               |
             * |                                                               |
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             */
			const auto nal_unit = nal[nal_index++];
			if (nal_unit->GetSize() <= MAX_RTP_PAYLOAD_SIZE) {
				const auto size = nal_unit->CopyData(
					rtp_packet_data, MAX_RTP_PAYLOAD_SIZE);
				rtp_packet.size = RTP_TCP_HEAD_SIZE +
						  RTP_HEADER_SIZE +
						  static_cast<uint16_t>(size);
				rtp_packet.last = 1;
				rtp_packet.type = GetRtpFrameType(nal_unit);
				if (!send_frame_callback_(channelId,
							  rtp_packet))
					return false;
			} else {
				//Fragmentation Units
				/*  0                   1                   2                   3
                 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * |PayloadHdr (28)|   FU header   |                               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
                 * |                                                               |
                 * |                                                               |
                 * |                         FU payload                            |
                 * |                                                               |
                 * |                                                               |
                 * |                                                               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
				 */
				//PayloadHeader
				nal_unit->CopyHeader(rtp_packet_data,
						     H264_NALU_HEADER_SIZE);
				rtp_packet_data[0] &= 0xe0;
				rtp_packet_data[0] |= 28;
				//FU Header
				rtp_packet_data[1] = nal_unit->GetType() & 0x1f;

				size_t skip = 0;
				const size_t size = nal_unit->GetBodySize();

				rtp_packet.size = RTP_TCP_HEAD_SIZE +
						  RTP_HEADER_SIZE +
						  MAX_RTP_PAYLOAD_SIZE;
				rtp_packet.last = 0;
				rtp_packet.type = GetRtpFrameType(nal_unit);

				//First
				rtp_packet_data[1] |= 0x80;
				skip += nal_unit->CopyBody(
					rtp_packet_data +
						H264_NALU_HEADER_SIZE + 1,
					MAX_RTP_PAYLOAD_SIZE -
						H264_NALU_HEADER_SIZE - 1,
					skip);
				if (!send_frame_callback_(channelId,
							  rtp_packet))
					return false;

				//Middle
				rtp_packet_data[1] &= 0x1f;
				while (size - skip >
				       MAX_RTP_PAYLOAD_SIZE -
					       H264_NALU_HEADER_SIZE - 1) {
					skip += nal_unit->CopyBody(
						rtp_packet_data +
							H264_NALU_HEADER_SIZE +
							1,
						MAX_RTP_PAYLOAD_SIZE -
							H264_NALU_HEADER_SIZE -
							1,
						skip);
					if (!send_frame_callback_(channelId,
								  rtp_packet))
						return false;
				}

				//Last
				rtp_packet_data[1] |= 0x40;
				rtp_packet.last = 1;
				const auto last_size = nal_unit->CopyBody(
					rtp_packet_data +
						H264_NALU_HEADER_SIZE + 1,
					MAX_RTP_PAYLOAD_SIZE -
						H264_NALU_HEADER_SIZE - 1,
					skip);
				rtp_packet.size =
					RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE +
					static_cast<uint16_t>(last_size) +
					H264_NALU_HEADER_SIZE + 1;
				if (!send_frame_callback_(channelId,
							  rtp_packet))
					return false;
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
	const auto time_point = chrono::time_point_cast<chrono::microseconds>(
		chrono::steady_clock::now());
	return static_cast<uint32_t>(
		(time_point.time_since_epoch().count() + 500) / 1000 * 90);
	//#endif
}

std::string H264Source::Base64Encode(const void *input, const size_t size)
FrameType H264Source::GetRtpFrameType(std::shared_ptr<NalUnit> nalUnit)
{
	std::vector<char> buffer(size / 3 * 4 + (size % 3 > 0 ? 4 : 0) + 1);
	base64_encodestate b64encoder;
	base64_init_encodestate(&b64encoder);

	const auto length = base64_encode_block(
		static_cast<const char *>(input), static_cast<int>(size),
		buffer.data(), &b64encoder);
	base64_encode_blockend(buffer.data() + length, &b64encoder);

	return std::string(buffer.cbegin(), buffer.cend() - 1); //TODO
	if (nalUnit->IsIdrFrame())
		return FrameType::VIDEO_FRAME_IDR;
	if (nalUnit->IsFrame())
		return FrameType::VIDEO_FRAME_NOTIDR;
	return FrameType::NONE;
}
