#include "BufferView.h"

#include <cassert>

namespace Zuazo::Utils {

template <typename T>
constexpr BufferView<T>::BufferView()
	: m_data(nullptr)
	, m_size(0)
{
}

template <typename T>
template <typename Q, typename, typename, typename>
constexpr BufferView<T>::BufferView(Q* data, size_t size)
	: m_data(static_cast<T*>(data))
	, m_size(size)
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(Q* begin, Q* end)
	: BufferView(begin, std::distance(begin, end))
{
}

template <typename T>
constexpr BufferView<T>:: BufferView(T& element)
	: BufferView(&element, 1)
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(Q (&arr)[N])
	: BufferView(arr, N)
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::initializer_list<Q> list)
	: BufferView(list.begin(), list.end())
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(const std::array<Q, N>& arr)
	: BufferView(arr.data(), arr.size())
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(std::array<Q, N>& arr)
	: BufferView(arr.data(), arr.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const std::vector<Q>& vec)
	: BufferView(vec.data(), vec.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::vector<Q>& vec)
	: BufferView(vec.data(), vec.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const std::valarray<Q>& vec)
	: BufferView(std::begin(vec), std::end(vec))
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::valarray<Q>& vec)
	: BufferView(std::begin(vec), std::end(vec))
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const BufferView<Q>& other)
	: BufferView(other.data(), other.size())
{
}

template <typename T>
constexpr BufferView<T>::operator bool() const {
	return m_size > 0;
}

template <typename T>
constexpr T* BufferView<T>::data() const {
	return m_data;
}

template <typename T>
constexpr size_t BufferView<T>::size() const {
	return m_size;
}

template <typename T>
constexpr T* BufferView<T>::begin() const {
	return data();
}

template <typename T>
constexpr T* BufferView<T>::cbegin() const {
	return begin();
}

template <typename T>
constexpr T* BufferView<T>::end() const {
	return data() + size();
}

template <typename T>
constexpr T* BufferView<T>::cend() const {
	return end();
}

template <typename T>
constexpr T& BufferView<T>::operator[](size_t i) const{
	assert(i < m_size);
	return m_data[i];
}

template <typename T>
constexpr T& BufferView<T>::front() const {
	return (*this)[0];
}

template <typename T>
constexpr T& BufferView<T>::back() const {
	return (*this)[m_size - 1];
}

}