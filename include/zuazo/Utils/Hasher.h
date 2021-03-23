#pragma once

#include "BufferView.h"

#include <tuple>
#include <cstdlib>

namespace Zuazo::Utils {

template <typename T, typename H = size_t>
struct Hasher {
	using value_type = T;
	using hash_type = H;

	constexpr hash_type operator()(const value_type& v) const noexcept;
};


template <typename... T, typename H>
struct Hasher<std::tuple<T...>, H> {
	using value_type = std::tuple<T...>;
	using hash_type = H;

	constexpr hash_type operator()(const value_type& v) const noexcept;

private:
	template<size_t... ids>
	constexpr hash_type tupleHash(const value_type& tuple, std::index_sequence<ids...>) const noexcept;

};


template <typename T, size_t N, typename H>
struct Hasher<std::array<T, N>, H> {
	using value_type = std::array<T, N>;
	using hash_type = H;

	constexpr hash_type operator()(const value_type& v) const noexcept;
};



template<typename T, typename H>
constexpr H hashCombine(const H& seed, const T& value) noexcept;

template <typename InputIt, typename H = size_t>
constexpr H hashAccumulate(InputIt begin, InputIt end) noexcept;

}

#include "Hasher.inl"