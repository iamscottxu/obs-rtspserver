#include "Nal.h"

using namespace std;
using namespace xop;

bool NalHelper::NalUnitWhile(const uint8_t *data, size_t dataSize,
			     NalUnitWhileCallback callback)
{
	if (dataSize == 0)
		return true;
	const uint8_t *end = data + dataSize;
	const uint8_t *nal_start = FindStartCode(data, end);

	while (true) {
		while (nal_start < end && !*nal_start++)
			;

		if (nal_start == end)
			break;

		const uint8_t *nalEnd = FindStartCode(nal_start, end);

		if (!callback(const_cast<uint8_t *>(nal_start),
			      nalEnd - nal_start))
			return false;

		nal_start = nalEnd;
	}
	return true;
}

uint32_t NalHelper::GetNalUnitCount(const uint8_t *data, size_t dataSize)
{
	uint32_t count = 0;
	NalUnitWhile(data, dataSize, [&count](const uint8_t *, size_t) {
		count++;
		return true;
	});
	return count;
}

const uint8_t *NalHelper::FindStartCode(const uint8_t *p, const uint8_t *end)
{
	const uint8_t *out = FFmpegFindStartcodeInternal(p, end);
	if (p < out && out < end && !out[-1])
		--out;
	return out;
}

/* NOTE: I noticed that FFmpeg does some unusual special handling of certain
 * scenarios that I was unaware of, so instead of just searching for {0, 0, 1}
 * we'll just use the code from FFmpeg - http://www.ffmpeg.org/ */
const uint8_t *NalHelper::FFmpegFindStartcodeInternal(const uint8_t *p,
						      const uint8_t *end)
{
	const uint8_t *a = p + 4 - (reinterpret_cast<intptr_t>(p) & 3);

	for (end -= 3; p < a && p < end; p++) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 1)
			return p;
	}

	for (end -= 3; p < end; p += 4) {
		const uint32_t x = *reinterpret_cast<const uint32_t *>(p);

		if (x - 0x01010101 & ~x & 0x80808080) {
			if (p[1] == 0) {
				if (p[0] == 0 && p[2] == 1)
					return p;
				if (p[2] == 0 && p[3] == 1)
					return p + 1;
			}

			if (p[3] == 0) {
				if (p[2] == 0 && p[4] == 1)
					return p + 2;
				if (p[4] == 0 && p[5] == 1)
					return p + 3;
			}
		}
	}

	for (end += 3; p < end; p++) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 1)
			return p;
	}

	return end + 3;
}
