#include "BufferView.h"

#include <cassert>

namespace Zuazo::Utils {

template <typename T>
constexpr BufferView<T>::BufferView() noexcept
	: m_begin(nullptr)
	, m_end(nullptr)
{
}

template <typename T>
template <typename Q, typename, typename, typename>
constexpr BufferView<T>::BufferView(Q* begin, Q* end) noexcept
	: m_begin(begin)
	, m_end(end)
{
}

template <typename T>
template <typename Q>
constexpr BufferView<T>::BufferView(Q* data, size_t size) noexcept
	: BufferView(data, data + size)
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
constexpr BufferView<T>::BufferView(std::initializer_list<T> list) noexcept
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
	: BufferView(other.begin(), other.end())
{
}

template <typename T>
constexpr BufferView<T>::operator bool() const noexcept {
	return !empty();
}

template <typename T>
constexpr T* BufferView<T>::data() const noexcept {
	return begin();
}

template <typename T>
constexpr size_t BufferView<T>::size() const noexcept {
	return std::distance(cbegin(), cend());
}

template <typename T>
constexpr T* BufferView<T>::begin() const noexcept {
	return m_begin;
}

template <typename T>
constexpr const T* BufferView<T>::cbegin() const noexcept {
	return begin();
}

template <typename T>
constexpr T* BufferView<T>::end() const noexcept {
	return m_end;
}

template <typename T>
constexpr const T* BufferView<T>::cend() const noexcept {
	return end();
}

template <typename T>
constexpr T& BufferView<T>::operator[](size_t i) const noexcept {
	assert(i < size());
	return data()[i];
}

template <typename T>
constexpr T& BufferView<T>::front() const noexcept {
	assert(!empty());
	return *(begin());
}

template <typename T>
constexpr T& BufferView<T>::back() const noexcept {
	assert(!empty());
	return *(end() - 1);
}

template <typename T>
constexpr bool BufferView<T>::empty() const noexcept {
	return size() == 0;
}

}