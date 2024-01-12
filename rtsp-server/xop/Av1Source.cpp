#if defined(WIN32) || defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "Av1Source.h"
#include <cstdio>
#include <cstring>
#include <chrono>
#include <utility>

#if defined(WIN32) || defined(_WIN32)

#else
#include <sys/time.h>
#endif

#include "Av1Tu.h"
#include "Av1Uleb128.h"
#include "net/Logger.h"

#define AV1_AGGREGATION_HEADER_SIZE 1
#define MAX_LEB128_SIZE             2

using namespace xop;
using namespace std;

Av1Source::Av1Source(std::shared_ptr<Av1ObuSequenceHeader> sequenceHeader, const uint32_t framerate)
	: framerate_(framerate),
	  sequence_header_(std::move(sequenceHeader))
{
	payload_ = 98;
	media_type_ = MediaType::AV1;
	clock_rate_ = 90000;
}

Av1Source *Av1Source::CreateNew(vector<uint8_t> extraData, const uint32_t framerate)
{
	Av1Tu obu(extraData);
	shared_ptr<Av1ObuSequenceHeader> sequence_header;
	vector<uint8_t> metadata;
	const auto sequence_header_unit =
		obu.GetObuByType(Av1ObuType::AV1_OBU_SEQUENCE_HEADER);
	if (sequence_header_unit != nullptr)
		sequence_header = sequence_header_unit->GetSequenceHeader();
	return new Av1Source(sequence_header, framerate);
}

Av1Source::~Av1Source() = default;

Av1Source *Av1Source::CreateNew(shared_ptr<Av1ObuSequenceHeader> sequenceHeader, const uint32_t framerate)
{
	return new Av1Source(std::move(sequenceHeader), framerate);
}

string Av1Source::GetMediaDescription(const uint16_t port)
{
	char buf[100] = {0};
	sprintf(buf, "m=video %hu RTP/AVP 98", port);

	return buf;
}

string Av1Source::GetAttribute()
{
	auto sdp = string("a=rtpmap:98 AV1/90000\r\n");

	if (sequence_header_ != nullptr) {
		const auto fmtp =
			"a=fmtp:98 profile=%u;level-idx=%u;tier=%u";

		const size_t buf_size = strlen(fmtp) + 1 + 3 + 1;
		auto buf = vector<char>(buf_size);

		snprintf(buf.data(), buf_size, fmtp,
			 sequence_header_->GetProfile(), sequence_header_->GetLevel(), sequence_header_->GetTier());
		sdp.append(buf.data());
	}

	return sdp;
}

bool Av1Source::HandleFrame(const MediaChannelId channelId,
			     const AVFrame frame)
{
	Av1Tu tu(frame.buffer.get(), frame.size);

	const auto obuCount = tu.GetCount();
	if (obuCount == 0) return false;

	size_t obuIndex = 0;
	size_t countSize = AV1_AGGREGATION_HEADER_SIZE;
	size_t partSkip = 0;
	size_t packetObuCount = 0;

	bool firstPacket = true;

	size_t currentUnitSize = tu[0]->GetNotHasSizeFieldSize();
	uint8_t currentUnitLeb128[MAX_LEB128_SIZE];
	size_t currentUnitLeb128Size =
		obuCount == 1 ? 0 : Av1Uleb128::EncodeUleb128(currentUnitSize, currentUnitLeb128,
							      sizeof(currentUnitLeb128));

	RtpPacket rtpPacket;
	rtpPacket.timestamp = frame.timestamp;
	rtpPacket.last = 0;
	rtpPacket.type = GetRtpFrameType(tu);
	const auto rtpPacketData =
		rtpPacket.data.get() + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE;

	// https://aomediacodec.github.io/av1-rtp-spec
	// 0                   1                   2                   3
	// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|V=2|0|0| CC=0  |M|     PT      |       sequence number         |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|                           timestamp                           |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|           synchronization source (SSRC) identifier            |
	//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	//| AV1 aggr hdr  |                                               |
	//+-+-+-+-+-+-+-+-+                                               |
	//|                                                               |
	//|                   Bytes 2..N of AV1 payload                   |
	//|                                                               |
	//|                                                               |
	//|                                                               |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	// AV1 Aggregation Header
	// 0 1 2 3 4 5 6 7
	//+-+-+-+-+-+-+-+-+
	//|Z|Y| W |N|-|-|-|
	//+-+-+-+-+-+-+-+-+
	// This function does not implement the situation when M is zero.
	rtpPacketData[0] = 0x00; // Init AV1 Aggregation Header

	auto copyLeb128 = [&]() {
		memcpy(rtpPacketData + countSize, currentUnitLeb128, currentUnitLeb128Size);
		countSize += currentUnitLeb128Size;
#ifdef DEBUG
		LOG_DEBUG("[AV1]Copy Leb128: Timestamp: %u\tPacketSize: %zu\tPacketObuCount: %zu\t"
			  "ObuIndex: %zu\tObuCount: %zu\tLeb128Size: %zu",
			  rtpPacket.timestamp, countSize, packetObuCount,
			  obuIndex, obuCount, currentUnitLeb128Size);
#endif
		return currentUnitLeb128Size;
	};

	auto copyObuData = [&](size_t _size, size_t _skip = 0) {
		_size = tu[obuIndex]->CopyNotHasSizeFieldData(rtpPacketData + countSize, _size, _skip);
		countSize += _size;
		packetObuCount++;
#ifdef DEBUG
		LOG_DEBUG("[AV1]Copy Obu:    Timestamp: %u\tPacketSize: %zu\tPacketObuCount: %zu\t"
			  "ObuIndex: %zu\tObuCount: %zu\tObuSize: %zu\tCopySize: %zu\tCopySkip: %zu",
			  rtpPacket.timestamp, countSize, packetObuCount,
			  obuIndex, obuCount, currentUnitSize, _size, _skip);
#endif
		return _size;
	};

	auto sendPacket = [&]() {
		if (firstPacket) rtpPacketData[0] |= 0x08; // AV1 AggrHeader: N = 1
		rtpPacketData[0] |= (packetObuCount << 4) & 0x30; // AV1 AggrHeader: W = packetObuCount (Max: 3)
		rtpPacket.size = RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + static_cast<uint16_t>(countSize);
		auto result = send_frame_callback_(channelId, rtpPacket);
#ifdef DEBUG
		LOG_DEBUG("[AV1]Send Packet: Timestamp: %u\tPacketSize: %zu\tPacketObuCount: %zu\t"
			  "AggrHdr: 0b%c%c%c%c%c%c%c%c\tIsMark: %s",
			  rtpPacket.timestamp, countSize, packetObuCount,
			  ((rtpPacketData[0] & 0x80) >> 7) + '0', ((rtpPacketData[0] & 0x40) >> 6) + '0',
			  ((rtpPacketData[0] & 0x20) >> 5) + '0', ((rtpPacketData[0] & 0x10) >> 4) + '0',
			  ((rtpPacketData[0] & 0x08) >> 3) + '0', ((rtpPacketData[0] & 0x04) >> 2) + '0',
			  ((rtpPacketData[0] & 0x02) >> 1) + '0', (rtpPacketData[0] & 0x01) + '0',
			  rtpPacket.last ? "Yes" : "No");
#endif
		firstPacket = false;
		packetObuCount = 0;
		rtpPacketData[0] = 0x00; // Init AV1 Aggregation Header
		countSize = AV1_AGGREGATION_HEADER_SIZE;
		return result;
	};

	auto getNextObu = [&]() {
		obuIndex++;
		if (obuIndex == obuCount) return;
		currentUnitSize = tu[obuIndex]->GetNotHasSizeFieldSize();
		if (obuIndex == obuCount - 1) {
			currentUnitLeb128Size = 0;
			return;
		}
		if (currentUnitSize >= MAX_RTP_PAYLOAD_SIZE - AV1_AGGREGATION_HEADER_SIZE) {
			currentUnitLeb128Size = MAX_LEB128_SIZE;
			return;
		}
		currentUnitLeb128Size = Av1Uleb128::EncodeUleb128(currentUnitSize, currentUnitLeb128,
									 sizeof(currentUnitLeb128));
	};

	while (obuIndex < obuCount) {
		// Situation 3: Three OBUs have been written into the RTP Packet.
		// An RTP Packet can only write to a maximum of three OBUs.
		// Send RTP Packet.
		if (packetObuCount == 3) {
			if (!sendPacket()) return false;
			continue;
		}

		// Situation 2: There is an OBU that has not been fully written.
		if (partSkip > 0) {
			auto partSize = currentUnitSize - partSkip;
			if (partSize == 0) {
				partSkip = 0;
				getNextObu();
				continue;
			}
			// Situation 2-1: The number of bytes that the OBU has not yet written to the
			// RTP Payload is still greater than the maximum number of bytes that the RTP
			// Payload can accommodate.
			// Write to OBU segmentally, write OBU middle segment, and send RTP Packet.
			if (partSize >= MAX_RTP_PAYLOAD_SIZE - AV1_AGGREGATION_HEADER_SIZE - MAX_LEB128_SIZE) {
				partSize = MAX_RTP_PAYLOAD_SIZE - AV1_AGGREGATION_HEADER_SIZE;
				partSize = copyObuData(partSize, partSkip);
				rtpPacketData[0] |= 0xc0; // AV1 AggrHeader: Z = 1, Y = 1
				if (!sendPacket()) return false;
				partSkip += partSize;
				continue;
			}
			// Situation 2-2: The number of bytes that the OBU has not yet written to the
			// RTP Payload less than the maximum number of bytes that the RTP Payload can
			// accommodate.
			// Write to OBU segmentally, write OBU end segment.
			{
				// Situation 2-2-1: Is the last OBU in the Temporal unit.
				// This must also be the last OBU in this RTP Packet.
				// Obviously it cannot have Size(LEB128) Header.

				// Situation 2-2-2: Not the last OBU in the Temporal unit.
				// There are OBU(s) that need to be written in the future,
				// which must have Size(LEB128) Header.
				if (obuIndex < obuCount - 1) {
					currentUnitLeb128Size = Av1Uleb128::EncodeUleb128(partSize, currentUnitLeb128,
											  sizeof(currentUnitLeb128));
					copyLeb128();
				}
				copyObuData(partSize, partSkip);
				rtpPacketData[0] |= 0x80; // AV1 AggrHeader: Z = 1, Y = 0
				partSkip = 0;
				getNextObu();
				continue;
			}
		}

		// Situation 1: RTP Payload is not enough to write OBU with Size(LEB128) header.
		if (countSize + currentUnitLeb128Size + currentUnitSize >= MAX_RTP_PAYLOAD_SIZE) {
			// Situation 1-2: RTP Payload is enough to write OBU without Size(LEB128) header.
			// Write OBU without Size(LEB128) header and send RTP Packet.
			// Note: The OBU without Size(LEB128) header must be the last OBU in the RTP Payload,
			// so even if there is space in the RTP Payload, it cannot continue to be written.
			if (countSize + currentUnitSize <= MAX_RTP_PAYLOAD_SIZE) {
				copyObuData(currentUnitSize);

				// Situation 1-2-1: Is the last OBU in the Temporal unit.
				// Then this is the last RTP Packer of this TU (Marker Packet).

				// Situation 1-2-2: Not the last OBU in the Temporal unit.
				// Then this is not the last RTP Packer of this TU (Marker Packet).
				if (obuIndex == obuCount - 1) rtpPacket.last = 1;
				if (!sendPacket()) return false;
				getNextObu();
				continue;
			}
			// Situation 1-1: RTP Payload is not enough to write OBU without Size(LEB128) header.
			// Write to OBU segmentally, write OBU start segment, and send RTP Packet.
			{
				size_t partSize = MAX_RTP_PAYLOAD_SIZE - countSize;
				partSize = copyObuData(MAX_RTP_PAYLOAD_SIZE - countSize);
				rtpPacketData[0] |= 0x40; // AV1 AggrHeader: Z = 0, Y = 1
				if (!sendPacket()) return false;
				partSkip = partSize;
				continue;
			}
		}

		// Situation 4: The remaining space of RTP Payload is enough to write
		// this OBU and Size(LEB128) header.
		// Write OBU.
		{
			// Situation 4-1: Is the last OBU in the Temporal unit.
			// This must also be the last OBU in this RTP Packet.
			// Obviously it cannot have Size(LEB128) Header.

			// Situation 4-2: Not the last OBU in the Temporal unit.
			// There are OBU(s) that need to be written in the future,
			// which must have Size(LEB128) Header.
			if (obuIndex < obuCount - 1) copyLeb128();
			copyObuData(currentUnitSize);
			getNextObu();
		}
	}

	// Situation 5: After the loop ends, there is still an RTP Packet that is not empty.
	// Send last RTP Packet (Marker Packet).
	if (countSize > AV1_AGGREGATION_HEADER_SIZE) {
		rtpPacket.last = 1;
		if (!sendPacket()) return false;
	}

	return true;
}

uint32_t Av1Source::GetTimestamp()
{
	/* #if defined(__linux) || defined(__linux__) 
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	uint32_t ts = ((tv.tv_sec*1000)+((tv.tv_usec+500)/1000))*90; // 90: _clockRate/1000;
	return ts;
#else */
	//auto time_point = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
	//auto time_point = chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now());
	const auto time_point = chrono::time_point_cast<chrono::microseconds>(
		chrono::steady_clock::now());
	return static_cast<uint32_t>(
		(time_point.time_since_epoch().count() + 500) / 1000 * 90);
	//#endif
}

FrameType Av1Source::GetRtpFrameType(const Av1Tu &obu)
{
	return FrameType::NONE;
	/*if (obu->IsIdrFrame())
		return FrameType::VIDEO_FRAME_IDR;
	if (obu->IsFrame())
		return FrameType::VIDEO_FRAME_NOTIDR;
	return FrameType::NONE;*/
}
