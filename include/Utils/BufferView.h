#pragma once

#include <type_traits>
#include <initializer_list>
#include <array>
#include <vector>

namespace Zuazo::Utils {

template <typename T>
class BufferView {
public:
	constexpr BufferView();

	template <	typename Q, 
				typename = typename std::enable_if< !std::is_same<T, void>::value>::type,
				typename = typename std::enable_if< !std::is_same<Q, void>::value>::type,
				typename = typename std::enable_if< std::is_convertible<Q(*)[], T(*)[]>::value>::type >
	constexpr BufferView(Q* data, size_t size);

	template <typename Q>
	constexpr BufferView(Q* begin, Q* end);

	template <typename Q, size_t N>
	constexpr BufferView(Q (&arr)[N]);

	template <typename Q>
	constexpr BufferView(std::initializer_list<Q> list);

	template <typename Q, size_t N>
	constexpr BufferView(std::array<Q, N>& arr);

	template <typename Q>
	constexpr BufferView(std::vector<Q>& vec);

	constexpr BufferView(const BufferView& other) = default;

	template <typename Q>
	constexpr BufferView(const BufferView<Q>& other);

	~BufferView() = default;


	constexpr BufferView&	operator=(const BufferView& other) = default;
	
	constexpr operator bool() const;

	constexpr T*			data() const;
	constexpr size_t		size() const;

	constexpr T&			operator[](size_t i) const;
	constexpr T&			front() const;
	constexpr T&			back() const;

private:
	T* 						m_data;
	size_t 					m_size;
};

}

#include "BufferView.inl"