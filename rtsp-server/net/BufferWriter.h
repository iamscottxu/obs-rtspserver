// PHZ
// 2018-5-15

#ifndef XOP_BUFFER_WRITER_H
#define XOP_BUFFER_WRITER_H

#include <cstdint>
#include <memory>
#include <queue>
#include "Socket.h"

namespace xop {

void WriteUint32BE(char *p, uint32_t value);
void WriteUint32LE(char *p, uint32_t value);
void WriteUint24BE(char *p, uint32_t value);
void WriteUint24LE(char *p, uint32_t value);
void WriteUint16BE(char *p, uint16_t value);
void WriteUint16LE(char *p, uint16_t value);

class BufferWriter {
public:
	explicit BufferWriter(int capacity = kMaxQueueLength);
	virtual ~BufferWriter() = default;

	bool Append(const std::shared_ptr<char> &data, size_t size,
		    uint32_t index = 0);
	bool Append(const char *data, size_t size, uint32_t index = 0);
	int Send(SOCKET sockfd, int timeout = 0);

	bool IsEmpty() const { return buffer_.empty(); }

	bool IsFull() const
	{
		return static_cast<int>(buffer_.size()) >= max_queue_length_
			       ? true
			       : false;
	}

	size_t Size() const { return buffer_.size(); }

private:
	typedef struct Packet {
		std::shared_ptr<char> data;
		size_t size{};
		uint32_t writeIndex{};
	} Packet;

	std::queue<Packet> buffer_;
	int max_queue_length_ = 0;

	static constexpr int kMaxQueueLength = 10000;
};

}

#endif
