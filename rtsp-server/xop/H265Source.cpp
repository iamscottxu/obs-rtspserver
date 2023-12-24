// PHZ
// 2018-6-7

#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "H265Source.h"
#include <cstdio>
#include <cstring>
#include <chrono>
#include <utility>

#if defined(WIN32) || defined(_WIN32)

#else
#include <sys/time.h>
#endif

#include "Base64Encode.h"
#include "Nal.h"
#include "H265NalUnit.h"

using namespace xop;
using namespace std;

H265Source::H265Source(vector<uint8_t> vps, vector<uint8_t> sps,
		       vector<uint8_t> pps, vector<uint8_t> sei,
		       const uint32_t framerate)
	: framerate_(framerate),
	  vps_(std::move(vps)),
	  sps_(std::move(sps)),
	  pps_(std::move(pps)),
	  sei_(std::move(sei))
{
	payload_ = 96;
	media_type_ = MediaType::H265;
	clock_rate_ = 90000;
}

H265Source *H265Source::CreateNew(vector<uint8_t> extraData,
				  vector<uint8_t> sei, const uint32_t framerate)
{
	Nal<H265NalUnit> nal(extraData);
	vector<uint8_t> vps, sps, pps;
	const auto vps_nal_unit = nal.GetNalUnitByType(
			   static_cast<uint8_t>(H265NalType::H265_NAL_VPS)),
		   sps_nal_unit = nal.GetNalUnitByType(
			   static_cast<uint8_t>(H265NalType::H265_NAL_SPS)),
		   pps_nal_unit = nal.GetNalUnitByType(
			   static_cast<uint8_t>(H265NalType::H265_NAL_PPS));
	if (vps_nal_unit != nullptr)
		vps = vps_nal_unit->GetData();
	if (sps_nal_unit != nullptr)
		sps = sps_nal_unit->GetData();
	if (pps_nal_unit != nullptr)
		pps = pps_nal_unit->GetData();

	return new H265Source(vps, sps, pps, std::move(sei), framerate);
}

H265Source::~H265Source() = default;

H265Source *H265Source::CreateNew(vector<uint8_t> vps, vector<uint8_t> sps,
				  vector<uint8_t> pps, vector<uint8_t> sei,
				  const uint32_t framerate)
{
	return new H265Source(std::move(vps), std::move(sps), std::move(pps), std::move(sei),
			      framerate);
}

string H265Source::GetMediaDescription(const uint16_t port)
{
	char buf[100];
	snprintf(buf, sizeof(buf), "m=video %hu RTP/AVP 96", port);

	return buf;
}

string H265Source::GetAttribute()
{
	auto sdp = string("a=rtpmap:96 H265/90000\r\n");

	if (!vps_.empty() && !sps_.empty() && !pps_.empty()) {
		const auto fmtp =
			"a=fmtp:96 profile-space=%u;tier-flag=%u;"
			"profile-id=%u;level-id=%u;interop-constraints=%012llX;"
			"sprop-vps=%s;sprop-pps=%s;sprop-sps=%s;%s";

		string vps_base64, pps_base64, sps_base64, sei;
		vps_base64 = Base64Encode(vps_.data(), vps_.size());
		pps_base64 = Base64Encode(pps_.data(), pps_.size());
		sps_base64 = Base64Encode(sps_.data(), sps_.size());
		if (!sei_.empty()) {
			sei = "sprop-sei=";
			sei.append(Base64Encode(sei_.data(), sei_.size()));
			sei.append(";");
		} else
			sei = "";

		const uint8_t profile_space = sps_.at(3) >> 6;
		const uint8_t tier_flag = (sps_.at(3) & 0x20) >> 5;
		const uint8_t profile_id = sps_.at(3) & 0x1f;
		const uint8_t level_id = sps_.at(17);
		const uint64_t interop_constraints =
			static_cast<uint64_t>(sps_.at(9)) << 40 |
			static_cast<uint64_t>(sps_.at(10)) << 32 |
			static_cast<uint64_t>(sps_.at(11)) << 24 |
			static_cast<uint64_t>(sps_.at(12)) << 16 |
			static_cast<uint64_t>(sps_.at(13)) << 8 |
			static_cast<uint64_t>(sps_.at(14));

		const size_t buf_size = 1 + strlen(fmtp) + vps_base64.length() +
					pps_base64.length() +
					sps_base64.length() + sei.length();
		auto buf = vector<char>(buf_size);

		snprintf(buf.data(), buf_size, fmtp, profile_space, tier_flag, profile_id,
			level_id, interop_constraints, vps_base64.c_str(),
			pps_base64.c_str(), sps_base64.c_str(), sei.c_str());
		buf[strlen(buf.data()) - 1] = '\0';

		sdp.append(buf.data());
	}

	return sdp;
}

bool H265Source::HandleFrame(const MediaChannelId channelId,
			     const AVFrame frame)
{
	RtpPacket rtp_packet;
	//rtp_packet.timestamp = frame.timestamp == 0 ? GetTimestamp() : frame.timestamp;
	rtp_packet.timestamp = frame.timestamp;
	const auto rtp_packet_data =
		rtp_packet.data.get() + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE;

	Nal<H265NalUnit> nal(frame.buffer.get(), frame.size);

	if (nal.GetCount() == 0)
		return false;

	size_t nal_index = 0;
	while (nal_index < nal.GetCount()) {
		size_t end_index = nal_index;
		size_t size_count = H265_NALU_HEADER_SIZE;
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
                 * |   PayloadHdr (Type=48)        |         NALU 1 Size           |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * |          NALU 1 HDR           |                               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+         NALU 1 Data           |
                 * |                   . . .                                       |
                 * |                                                               |
                 * +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * |  . . .        | NALU 2 Size                   | NALU 2 HDR    |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * | NALU 2 HDR    |                                               |
                 * +-+-+-+-+-+-+-+-+              NALU 2 Data                      |
                 * |                   . . .                                       |
                 * |                                                               |
                 * |                                                               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 */
			rtp_packet.size = RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE +
					  static_cast<uint16_t>(size_count);
			rtp_packet.last = 1;
			size_t skip = H265_NALU_HEADER_SIZE;
			uint8_t lowest_layer_id = 0x3f;
			uint8_t lowest_temporal_id = 0x07;
			auto all_frame_type = FrameType::NONE;
			for (; nal_index <= end_index; nal_index++) {
				const auto nal_unit = nal[nal_index];
				const auto layer_id = nal_unit->GetLayerId();
				const auto temporal_id =
					nal_unit->GetTemporalId();
				const auto frame_type =
					GetRtpFrameType(nal_unit);
				if (lowest_layer_id > layer_id)
					lowest_layer_id = layer_id;
				if (lowest_temporal_id > temporal_id)
					lowest_temporal_id = temporal_id;
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
						H265_NALU_HEADER_SIZE - 2);
			}
			//PayloadHeader
			rtp_packet_data[0] = (lowest_layer_id & 0x3f) >> 5 |
					     48 << 1;
			rtp_packet_data[1] = static_cast<uint8_t>(
				(lowest_layer_id & 0x3f) << 3 |
				(lowest_temporal_id & 0x07));
			rtp_packet.type = all_frame_type;
			if (!send_frame_callback_(channelId, rtp_packet))
				return false;
		} else {
		//Single NAL Unit Packets
		/*  0                   1                   2                   3
                 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * |           PayloadHdr          |                               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
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
                 * |    PayloadHdr (Type=49)       |   FU header   |               |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
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
						     H265_NALU_HEADER_SIZE);
				rtp_packet_data[0] &= 0x81;
				rtp_packet_data[0] |= 49 << 1;
				//FU Header
				rtp_packet_data[2] = nal_unit->GetType() & 0x3f;

				size_t skip = 0;
				const size_t size = nal_unit->GetBodySize();

				rtp_packet.size = RTP_TCP_HEAD_SIZE +
						  RTP_HEADER_SIZE +
						  MAX_RTP_PAYLOAD_SIZE;
				rtp_packet.last = 0;
				rtp_packet.type = GetRtpFrameType(nal_unit);

				//First
				rtp_packet_data[2] |= 0x80;
				skip += nal_unit->CopyBody(
					rtp_packet_data +
						H265_NALU_HEADER_SIZE + 1,
					MAX_RTP_PAYLOAD_SIZE -
						H265_NALU_HEADER_SIZE - 1,
					skip);
				if (!send_frame_callback_(channelId,
							  rtp_packet))
					return false;

				//Middle
				rtp_packet_data[2] &= 0x3f;
				while (size - skip > MAX_RTP_PAYLOAD_SIZE - 3) {
					skip += nal_unit->CopyBody(
						rtp_packet_data +
							H265_NALU_HEADER_SIZE +
							1,
						MAX_RTP_PAYLOAD_SIZE -
							H265_NALU_HEADER_SIZE -
							1,
						skip);
					if (!send_frame_callback_(channelId,
								  rtp_packet))
						return false;
				}

				//Last
				rtp_packet_data[2] |= 0x40;
				rtp_packet.last = 1;
				const auto last_size = nal_unit->CopyBody(
					rtp_packet_data +
						H265_NALU_HEADER_SIZE + 1,
					MAX_RTP_PAYLOAD_SIZE -
						H265_NALU_HEADER_SIZE - 1,
					skip);
				rtp_packet.size =
					RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE +
					static_cast<uint16_t>(last_size) +
					H265_NALU_HEADER_SIZE + 1;
				if (!send_frame_callback_(channelId,
							  rtp_packet))
					return false;
			}
		}
	}
	return true;
}

uint32_t H265Source::GetTimestamp()
{
	/* #if defined(__linux) || defined(__linux__) 
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	uint32_t ts = ((tv.tv_sec*1000)+((tv.tv_usec+500)/1000))*90; // 90: _clockRate/1000;
	return ts;
#else */
	//auto time_point = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
	const auto time_point = chrono::time_point_cast<chrono::microseconds>(
		chrono::steady_clock::now());
	return static_cast<uint32_t>(
		(time_point.time_since_epoch().count() + 500) / 1000 * 90);
	//#endif
}

FrameType H265Source::GetRtpFrameType(std::shared_ptr<NalUnit> nalUnit)
{
	if (nalUnit->IsIdrFrame())
		return FrameType::VIDEO_FRAME_IDR;
	if (nalUnit->IsFrame())
		return FrameType::VIDEO_FRAME_NOTIDR;
	return FrameType::NONE;
}
