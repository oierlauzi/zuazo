#pragma once

#include "BufferView.h"

#include <vector>

namespace Zuazo::Utils {

class Area {
public:
	constexpr Area();
	constexpr Area(size_t offset, size_t size);
	constexpr Area(const Area& other) = default;
	~Area() = default;

	constexpr Area&		operator=(const Area& other) = default;

	constexpr bool		operator==(const Area& other) const;
	constexpr bool		operator!=(const Area& other) const;
	constexpr bool		operator<(const Area& other) const;
	constexpr bool		operator<=(const Area& other) const;
	constexpr bool		operator>(const Area& other) const;
	constexpr bool		operator>=(const Area& other) const;

	constexpr size_t	offset() const;
	constexpr size_t	size() const;
	constexpr size_t	end() const;
	constexpr size_t	elementOffset(size_t elementSize) const;
	constexpr size_t	elementCount(size_t elementSize) const;


	template <typename T>
	constexpr size_t	elementOffset() const;

	template <typename T>
	constexpr size_t	elementCount() const;

	template <typename T>
	constexpr T*		begin(T* data) const;

	template <typename T>
	constexpr T*		end(T* data) const;

	template <typename T>
	constexpr BufferView<T> apply(T* data) const;

	template <typename T>
	constexpr BufferView<T> apply(BufferView<T> data) const;

private:
	size_t				m_offset;
	size_t				m_size;

};

template<typename T>
constexpr void slice(T* data, BufferView<const Area> areas, BufferView<BufferView<T>> result);

template<typename T>
std::vector<BufferView<T>> slice(T* data, BufferView<const Area> areas);

}

#include "Area.inl"