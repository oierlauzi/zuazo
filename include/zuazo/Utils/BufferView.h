#pragma once

#include <type_traits>
#include <initializer_list>
#include <array>
#include <vector>
#include <valarray>

namespace Zuazo::Utils {

template <typename T>
class BufferView {
public:
	constexpr BufferView() noexcept;

	template <	typename Q, 
				typename = typename std::enable_if< !std::is_same<T, void>::value>::type,
				typename = typename std::enable_if< !std::is_same<Q, void>::value>::type,
				typename = typename std::enable_if< std::is_convertible<Q(*)[], T(*)[]>::value>::type >
	constexpr BufferView(Q* data, size_t size) noexcept;

	template <typename Q>
	constexpr BufferView(Q* begin, Q* end) noexcept;
	
	constexpr BufferView(T& element) noexcept;

	template <typename Q, size_t N>
	constexpr BufferView(Q (&arr)[N]) noexcept;

	template <typename Q>
	constexpr BufferView(std::initializer_list<Q> list) noexcept;

	template <typename Q, size_t N>
	constexpr BufferView(const std::array<Q, N>& arr) noexcept;

	template <typename Q, size_t N>
	constexpr BufferView(std::array<Q, N>& arr) noexcept;

	template <typename Q>
	constexpr BufferView(const std::vector<Q>& vec) noexcept;

	template <typename Q>
	constexpr BufferView(std::vector<Q>& vec) noexcept;

	template <typename Q>
	constexpr BufferView(const std::valarray<Q>& vec) noexcept;

	template <typename Q>
	constexpr BufferView(std::valarray<Q>& vec) noexcept;

	constexpr BufferView(const BufferView& other) noexcept = default;

	template <typename Q>
	constexpr BufferView(const BufferView<Q>& other) noexcept;

	~BufferView() = default;


	constexpr BufferView&	operator=(const BufferView& other) noexcept = default;
	
	constexpr operator bool() const noexcept;

	constexpr T*			data() const noexcept;
	constexpr size_t		size() const noexcept;

	constexpr T*			begin() const noexcept;
	constexpr T*			cbegin() const noexcept;
	constexpr T*			end() const noexcept;
	constexpr T*			cend() const noexcept;

	constexpr T&			operator[](size_t i) const noexcept;
	constexpr T&			front() const noexcept;
	constexpr T&			back() const noexcept;

private:
	T* 						m_data;
	size_t 					m_size;
};

}

#include "BufferView.inl"