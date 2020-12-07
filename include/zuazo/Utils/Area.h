#pragma once

#include "BufferView.h"

#include <vector>

namespace Zuazo::Utils {

class Area {
public:
	constexpr Area() noexcept;
	constexpr Area(size_t offset, size_t size) noexcept;
	constexpr Area(const Area& other) noexcept = default;
	~Area() = default;

	constexpr Area&		operator=(const Area& other) noexcept = default;

	constexpr bool		operator==(const Area& other) const noexcept;
	constexpr bool		operator!=(const Area& other) const noexcept;
	constexpr bool		operator<(const Area& other) const noexcept;
	constexpr bool		operator<=(const Area& other) const noexcept;
	constexpr bool		operator>(const Area& other) const noexcept;
	constexpr bool		operator>=(const Area& other) const noexcept;

	constexpr Area		operator|(const Area& other) const noexcept;
	constexpr Area&		operator|=(const Area& other) noexcept;
	constexpr Area		operator&(const Area& other) const noexcept;
	constexpr Area&		operator&=(const Area& other) noexcept;

	constexpr size_t	offset() const noexcept;
	constexpr size_t	size() const noexcept;
	constexpr size_t	begin() const noexcept;
	constexpr size_t	end() const noexcept;
	constexpr size_t	elementOffset(size_t elementSize) const noexcept;
	constexpr size_t	elementCount(size_t elementSize) const noexcept;


	template <typename T>
	constexpr size_t	elementOffset() const noexcept;
	template <typename T>
	constexpr size_t	elementCount() const noexcept;

	template <typename T>
	constexpr T*		begin(T* data) const noexcept;

	template <typename T>
	constexpr T*		end(T* data) const noexcept;

	template <typename T>
	constexpr BufferView<T> apply(T* data) const noexcept;

	template <typename T>
	constexpr BufferView<T> apply(BufferView<T> data) const noexcept;

private:
	size_t				m_offset;
	size_t				m_size;

};

template<typename T>
constexpr void slice(T* data, BufferView<const Area> areas, BufferView<BufferView<T>> result) noexcept;

template<typename T>
std::vector<BufferView<T>> slice(T* data, BufferView<const Area> areas);

}

#include "Area.inl"