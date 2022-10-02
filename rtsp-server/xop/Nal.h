#ifndef _XOP_NAL_H
#define _XOP_NAL_H

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>
#include <cstdint>

#include "NalUnit.h"

namespace xop {

template<class T, class = std::enable_if_t<std::is_base_of_v<NalUnit, T>>>
class Nal {
public:
	Nal(const std::vector<uint8_t> &data);
	Nal(const uint8_t *data, size_t dataSize);
	size_t GetSize() const;
	size_t GetData(uint8_t **data) const;
	std::vector<uint8_t> GetData() const;
	std::shared_ptr<T> operator[](size_t index);
	size_t GetCount();
	size_t CopyData(uint8_t *start, size_t size) const;
	std::shared_ptr<T> GetNalUnitByType(uint8_t nalUnitType);

private:
	const uint8_t *data_;
	size_t data_size_;
	std::vector<std::shared_ptr<T>> nal_unit_list_;
};

class NalHelper {
public:
	using NalUnitWhileCallback =
		std::function<bool(const uint8_t *data, size_t dataSize)>;
	static bool NalUnitWhile(const uint8_t *data, size_t dataSize,
				 NalUnitWhileCallback callback);
	static uint32_t GetNalUnitCount(const uint8_t *data, size_t dataSize);
	static const uint8_t *FindStartCode(const uint8_t *p,
					    const uint8_t *end);

private:
	NalHelper() = default;
	static const uint8_t *FFmpegFindStartcodeInternal(const uint8_t *p,
							  const uint8_t *end);
};

template<class T, class T0>
Nal<T, T0>::Nal(const std::vector<uint8_t> &data) : Nal(data.data(), data.size())
{
}

template<class T, class T0>
Nal<T, T0>::Nal(const uint8_t *data, size_t dataSize)
	: data_(data), data_size_(dataSize)
{
	nal_unit_list_.resize(NalHelper::GetNalUnitCount(data, dataSize));
	auto it = nal_unit_list_.begin();
	auto end = nal_unit_list_.end();
	NalHelper::NalUnitWhile(
		data, dataSize,
		[&it, end](const uint8_t *unitData, size_t unitDataSize) {
			*it = shared_ptr<T>(static_cast<T *>(
				T::GetNalUnit(unitData, unitDataSize)));
			if (it++ == end)
				return false;
			return true;
		});
}

template<class T, class T0> size_t Nal<T, T0>::GetSize() const
{
	return data_size_;
}

template<class T, class T0> size_t Nal<T, T0>::GetData(uint8_t **data) const
{
	*data = const_cast<uint8_t *>(data_);
	return data_size_;
}

template<class T, class T0> std::vector<uint8_t> Nal<T, T0>::GetData() const
{
	uint8_t *data = nullptr;
	const auto size = GetData(&data);
	return std::vector<uint8_t>(data, data + size);
}

template<class T, class T0>
std::shared_ptr<T> Nal<T, T0>::operator[](size_t index)
{
	return nal_unit_list_[index];
}

template<class T, class T0> size_t Nal<T, T0>::GetCount()
{
	return nal_unit_list_.size();
}

template<class T, class T0>
size_t Nal<T, T0>::CopyData(uint8_t *start, size_t size) const
{
	if (size > data_size_)
		size = data_size_;
	memcpy(start, data_, size);
	return size;
}

template<class T, class T0>
std::shared_ptr<T> Nal<T, T0>::GetNalUnitByType(uint8_t nalUnitType)
{
	for (auto iter = nal_unit_list_.begin(); iter != nal_unit_list_.end();
	     ++iter) {
		if (iter->get()->GetType() == nalUnitType)
			return *iter;
	}
	return nullptr;
}

}
#endif
