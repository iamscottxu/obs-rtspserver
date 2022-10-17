#ifndef XOP_H264_PARSER_H
#define XOP_H264_PARSER_H

#include <cstdint>
#include <utility>
#include <vector>

namespace xop {

typedef std::pair<uint8_t *, uint8_t *> Nal; // <nal begin, nal end>

class H264Parser {
public:
	static Nal findNal(const uint8_t *data, uint32_t size);

	static std::vector<uint8_t>
	RemoveEmulationBytes(std::vector<uint8_t> const &from);
};

}

#endif
