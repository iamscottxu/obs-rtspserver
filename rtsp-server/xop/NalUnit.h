#ifndef _XOP_NALUNIT_H
#define _XOP_NALUNIT_H

#include <cstdint>
#include <vector>

namespace xop {
class NalUnit {
public:
	size_t GetData(uint8_t **data) const;
	std::vector<uint8_t> GetData() const;
	size_t CopyData(uint8_t *start, size_t size, size_t skip = 0) const;
	size_t GetSize() const;
	virtual uint8_t GetType() = 0;
	virtual size_t GetHeader(uint8_t **data) = 0;
	std::vector<uint8_t> GetHeader();
	virtual size_t CopyHeader(uint8_t *start, size_t size) = 0;
	virtual size_t GetHeaderSize() = 0;
	virtual size_t GetBody(uint8_t **data) = 0;
	std::vector<uint8_t> GetBody();
	virtual size_t CopyBody(uint8_t *start, size_t size, size_t skip = 0) = 0;
	virtual size_t GetBodySize() = 0;
	virtual bool IsIdrFrame() = 0;
	virtual bool IsFrame() = 0;
	static NalUnit* GetNalUnit(const uint8_t *data, size_t dataSize);

protected:
	NalUnit(const uint8_t *data, size_t dataSize);

private:
	const uint8_t *data_;
	size_t data_size_;
};
}

#endif
