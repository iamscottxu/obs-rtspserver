#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "Av1Obu.h"

namespace xop {

class Av1Tu {
public:
	Av1Tu(const std::vector<uint8_t> &data);
	Av1Tu(const uint8_t *data, size_t dataSize);
	size_t GetSize() const;
	size_t GetData(uint8_t **data) const;
	std::vector<uint8_t> GetData() const;
	std::shared_ptr<Av1Obu> operator[](size_t index);
	size_t GetCount();
	size_t CopyData(uint8_t *start, size_t size) const;
	std::shared_ptr<Av1Obu> GetObuByType(Av1ObuType obuType);

private:
	static size_t GetTuObuCount(const uint8_t *data, size_t dataSize);

	const uint8_t *data_;
	size_t data_size_;
	std::vector<std::shared_ptr<Av1Obu>> obu_list_;
};

}