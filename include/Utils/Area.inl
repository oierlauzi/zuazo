#include "Area.h"

#include <cassert>
#include <tuple>

namespace Zuazo::Utils {

constexpr Area::Area()
	: Area(0, 0)
{
}

constexpr Area::Area(size_t offset, size_t size)
	: m_offset(offset)
	, m_size(size)
{
}



constexpr bool Area::operator==(const Area& other) const {
	return std::tie(m_offset, m_size) == std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator!=(const Area& other) const {
	return std::tie(m_offset, m_size) != std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator<(const Area& other) const {
	return std::tie(m_offset, m_size) < std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator<=(const Area& other) const {
	return std::tie(m_offset, m_size) <= std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator>(const Area& other) const {
	return std::tie(m_offset, m_size) > std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator>=(const Area& other) const {
	return std::tie(m_offset, m_size) >= std::tie(other.m_offset, other.m_size);
}



constexpr size_t Area::offset() const {
	return m_offset;
}

constexpr size_t Area::size() const {
	return m_size;
}

constexpr size_t Area::end() const {
	return m_offset + m_size;
}

constexpr size_t Area::elementOffset(size_t elementSize) const {
	assert(m_offset % elementSize == 0);
	return m_offset / elementSize;
}

constexpr size_t Area::elementCount(size_t elementSize) const {
	assert(m_size % elementSize == 0);
	return m_size / elementSize;
}


template <typename T>
constexpr size_t Area::elementOffset() const {
	return elementOffset(sizeof(T));
}

template <typename T>
constexpr size_t Area::elementCount() const {
	return elementCount(sizeof(T));
}

template <typename T>
constexpr T* Area::begin(T* data) const {
	return data + elementOffset<T>();
}

template <typename T>
constexpr T* Area::end(T* data) const {
	return begin(data) + elementCount<T>();
}

template <typename T>
constexpr BufferView<T> Area::apply(T* data) const {
	return BufferView<T>(begin(data), elementCount<T>());
}

template <typename T>
constexpr BufferView<T> Area::apply(BufferView<T> data) const {
	return apply(data.data());
}



template<typename T>
constexpr void slice(T* data, BufferView<const Area> areas, BufferView<BufferView<T>> result) {
	assert(areas.size() == result.size());
	for(size_t i = 0; i < result.size(); i++) {
		result[i] = areas[i].apply(data);
	}
}

template<typename T>
inline std::vector<BufferView<T>> slice(T* data, BufferView<const Area> areas) {
	std::vector<BufferView<T>> result(areas.size());
	slice(data, areas, BufferView<BufferView<T>>(result));
	return result;
}

}