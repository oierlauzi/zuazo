#pragma once

#include <cstddef>
#include <array>
#include <vector>

namespace Zuazo::Utils {

template <typename T>
class BufferView {
public:
	constexpr BufferView() = default;
	constexpr BufferView(const T& element);
	constexpr BufferView(const T* data, size_t size);
	template<size_t L>
	constexpr BufferView(const std::array<T, L>& array);
	BufferView(const std::vector<T>& vector);
	constexpr BufferView(const BufferView& other) = default;
	~BufferView() = default;

	constexpr BufferView& operator=(const BufferView& other) = default;

	constexpr operator bool() const;

	constexpr size_t 	size() const;
	constexpr const T* 	data() const;
private:
	const T* 			m_data;
	size_t 				m_size;
};

}

#include "BufferView.inl"