#include "Av1Tu.h"
#include "net/Logger.h"
#include "Av1Uleb128.h"

using namespace std;
using namespace xop;

Av1Tu::Av1Tu(const std::vector<uint8_t> &data) : Av1Tu(data.data(), data.size())
{
}

Av1Tu::Av1Tu(const uint8_t *data, size_t dataSize)
	: data_(data), data_size_(dataSize)
{
	obu_list_.reserve(GetTuObuCount(data_, data_size_));

	auto it = data_;
	auto end = data_ + data_size_;
	while (it < end) {
		auto obu = std::shared_ptr<Av1Obu>(
			Av1Obu::GetAv1ObuUnit(it, end - it));
		auto obuSize = obu->GetSize();
		if (obuSize == 0) {
			it += 1;
			LOG_WARNING("[AV1]Unable to parse obu, skipped.");
		} else it += obuSize;
		auto obuType = obu->GetType();
		if (obuType == Av1ObuType::AV1_OBU_TEMPORAL_DELIMITER ||
		    obuType == Av1ObuType::AV1_OBU_TILE_LIST ||
		    obuType == Av1ObuType::AV1_OBU_PADDING)
			continue;
		obu_list_.push_back(obu);
	}
}

size_t Av1Tu::GetSize() const
{
	return data_size_;
}

size_t Av1Tu::GetData(uint8_t **data) const
{
	*data = const_cast<uint8_t *>(data_);
	return data_size_;
}

std::vector<uint8_t> Av1Tu::GetData() const
{
	uint8_t *data = nullptr;
	const auto size = GetData(&data);
	return std::vector<uint8_t>(data, data + size);
}


std::shared_ptr<Av1Obu> Av1Tu::operator[](size_t index)
{
	return obu_list_[index];
}

size_t Av1Tu::GetCount()
{
	return obu_list_.size();
}

size_t Av1Tu::CopyData(uint8_t *start, size_t size) const
{
	if (size > data_size_)
		size = data_size_;
	memcpy(start, data_, size);
	return size;
}

std::shared_ptr<Av1Obu> Av1Tu::GetObuByType(Av1ObuType obuType)
{
	for (auto iter = obu_list_.begin(); iter != obu_list_.end();
	     ++iter) {
		if (iter->get()->GetType() == obuType)
			return *iter;
	}
	return nullptr;
}

size_t Av1Tu::GetTuObuCount(const uint8_t *data, size_t dataSize)
{
	size_t count = 0;
	auto it = data;
	auto end = data + dataSize;
	while (it < end) {
		if (((it[0] & 0x80) >> 7) != 0) {
			it++;
			continue;
		}
		if ((it[0] &0x02) >> 1 != 1) {
			LOG_WARNING(
				"[AV1]The obu in the temporal unit does not have a size field, "
				"and the remaining data will be put into the last obu.");
			break;
		}
		it++;
		if (it == end - 1) break;
		size_t obuBodySize;
		it += Av1Uleb128::DecodeUleb128(it, end - it, &obuBodySize);
		it += obuBodySize;
		count++;
	}
	return count;
}
