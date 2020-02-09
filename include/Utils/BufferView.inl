#include "BufferView.h"

namespace Zuazo::Utils {

template <typename T>
constexpr BufferView<T>::BufferView(const T* data, size_t size)
	: m_data(data)
	, m_size(size)
{
}

template <typename T>
template<size_t L>
constexpr BufferView<T>::BufferView(const std::array<T, L>& array)
	: BufferView(array.data(), array.size())
{
}

template <typename T>
inline BufferView<T>::BufferView(const std::vector<T>& vector)
	: BufferView(vector.data(), vector.size())
{
}

template <typename T>
constexpr BufferView<T>::operator bool() const{
	return (m_size > 0);
}

template <typename T>
constexpr size_t BufferView<T>::size() const{
	return m_size;
}

template <typename T>
constexpr const T* BufferView<T>::data() const{
	return m_data;
}

}