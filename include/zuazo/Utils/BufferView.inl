#include "BufferView.h"

#include <cassert>

namespace Zuazo::Utils {

template <typename T>
constexpr BufferView<T>::BufferView() noexcept
	: m_data(nullptr)
	, m_size(0)
{
}

template <typename T>
template <typename Q, typename, typename, typename>
constexpr BufferView<T>::BufferView(Q* data, size_t size) noexcept
	: m_data(static_cast<T*>(data))
	, m_size(size)
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(Q* begin, Q* end) noexcept
	: BufferView(begin, std::distance(begin, end))
{
}

template <typename T>
constexpr BufferView<T>:: BufferView(T& element) noexcept
	: BufferView(&element, 1)
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(Q (&arr)[N]) noexcept
	: BufferView(arr, N)
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::initializer_list<Q> list) noexcept
	: BufferView(list.begin(), list.end())
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(const std::array<Q, N>& arr) noexcept
	: BufferView(arr.data(), arr.size())
{
}

template <typename T>
template <typename Q, size_t N>
constexpr BufferView<T>::BufferView(std::array<Q, N>& arr) noexcept
	: BufferView(arr.data(), arr.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const std::vector<Q>& vec) noexcept
	: BufferView(vec.data(), vec.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::vector<Q>& vec) noexcept
	: BufferView(vec.data(), vec.size())
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const std::valarray<Q>& vec) noexcept
	: BufferView(std::begin(vec), std::end(vec))
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(std::valarray<Q>& vec) noexcept
	: BufferView(std::begin(vec), std::end(vec))
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(const BufferView<Q>& other) noexcept
	: BufferView(other.data(), other.size())
{
}

template <typename T>
constexpr BufferView<T>::operator bool() const noexcept {
	return m_size > 0;
}

template <typename T>
constexpr T* BufferView<T>::data() const noexcept {
	return m_data;
}

template <typename T>
constexpr size_t BufferView<T>::size() const noexcept {
	return m_size;
}

template <typename T>
constexpr T* BufferView<T>::begin() const noexcept {
	return data();
}

template <typename T>
constexpr T* BufferView<T>::cbegin() const noexcept {
	return begin();
}

template <typename T>
constexpr T* BufferView<T>::end() const noexcept {
	return data() + size();
}

template <typename T>
constexpr T* BufferView<T>::cend() const noexcept {
	return end();
}

template <typename T>
constexpr T& BufferView<T>::operator[](size_t i) const noexcept {
	assert(i < m_size);
	return m_data[i];
}

template <typename T>
constexpr T& BufferView<T>::front() const noexcept {
	return (*this)[0];
}

template <typename T>
constexpr T& BufferView<T>::back() const noexcept {
	return (*this)[m_size - 1];
}

}