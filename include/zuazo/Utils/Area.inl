#include "Area.h"

#include <cassert>
#include <tuple>

namespace Zuazo::Utils {

constexpr Area::Area() noexcept
	: Area(0, 0)
{
}

constexpr Area::Area(size_t offset, size_t size) noexcept
	: m_offset(offset)
	, m_size(size)
{
}



constexpr bool Area::operator==(const Area& other) const noexcept{
	return std::tie(m_offset, m_size) == std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator!=(const Area& other) const noexcept {
	return std::tie(m_offset, m_size) != std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator<(const Area& other) const noexcept {
	return std::tie(m_offset, m_size) < std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator<=(const Area& other) const noexcept {
	return std::tie(m_offset, m_size) <= std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator>(const Area& other) const noexcept {
	return std::tie(m_offset, m_size) > std::tie(other.m_offset, other.m_size);
}

constexpr bool Area::operator>=(const Area& other) const noexcept {
	return std::tie(m_offset, m_size) >= std::tie(other.m_offset, other.m_size);
}



constexpr size_t Area::offset() const noexcept {
	return m_offset;
}

constexpr size_t Area::size() const noexcept {
	return m_size;
}

constexpr size_t Area::end() const noexcept {
	return m_offset + m_size;
}

constexpr size_t Area::elementOffset(size_t elementSize) const noexcept {
	return m_offset / elementSize;
}

constexpr size_t Area::elementCount(size_t elementSize) const noexcept {
	return m_size / elementSize;
}


template <typename T>
constexpr size_t Area::elementOffset() const noexcept {
	return elementOffset(sizeof(T));
}

template <typename T>
constexpr size_t Area::elementCount() const noexcept {
	return elementCount(sizeof(T));
}

template <typename T>
constexpr T* Area::begin(T* data) const noexcept {
	return data + elementOffset<T>();
}

template <typename T>
constexpr T* Area::end(T* data) const noexcept {
	return begin(data) + elementCount<T>();
}

template <typename T>
constexpr BufferView<T> Area::apply(T* data) const noexcept {
	return BufferView<T>(begin(data), elementCount<T>());
}

template <typename T>
constexpr BufferView<T> Area::apply(BufferView<T> data) const noexcept {
	return apply(data.data());
}



template<typename T>
constexpr void slice(T* data, BufferView<const Area> areas, BufferView<BufferView<T>> result) noexcept {
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