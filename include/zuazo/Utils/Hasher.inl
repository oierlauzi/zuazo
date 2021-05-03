#include "Hasher.h"

#include <utility>
#include <numeric>

namespace Zuazo::Utils {

template <typename T, typename H>
constexpr typename Hasher<T, H>::hash_type 
Hasher<T, H>::operator()(const value_type& v) const noexcept {
	constexpr std::hash<value_type> hasher;
	return static_cast<H>(hasher(v));
}



template <typename... T, typename H>
constexpr typename Hasher<std::tuple<T...>, H>::hash_type 
Hasher<std::tuple<T...>, H>::operator()(const value_type& v) const noexcept {
	return tupleHash(v, std::make_index_sequence<std::tuple_size<value_type>::value>());
}

template <typename... T, typename H>
template<size_t... ids>
constexpr typename Hasher<std::tuple<T...>, H>::hash_type 
Hasher<std::tuple<T...>, H>::tupleHash(const value_type& tuple, std::index_sequence<ids...>) const noexcept {
	//Based on:
	//https://codereview.stackexchange.com/questions/136770/hashing-a-tuple-in-c17
	//Compute the hashes for all the elements in the tuple
	const std::array<hash_type, sizeof...(ids)> hashes = {
		Hasher<typename std::tuple_element<ids, value_type>::type, hash_type>()(
			std::get<ids>(tuple)
		)
		...
	};

	//Combine the hashes. 
	return hashAccumulate(hashes.cbegin(), hashes.cend());
}



template <typename T, size_t N, typename H>
constexpr typename Hasher<std::array<T, N>, H>::hash_type 
Hasher<std::array<T, N>, H>::operator()(const value_type& v) const noexcept {
	return hashAccumulate(v.cbegin(), v.cend());
}



template<typename T, typename H>
constexpr H hashCombine(const H& seed, const T& value) noexcept {
	//Based on:
	//Boost's hash_combine
	constexpr Hasher<T, H> hasher;
	return seed ^ (hasher(value) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

template <typename InputIt, typename H>
constexpr H hashAccumulate(InputIt begin, InputIt end) noexcept {
	return std::accumulate(
		begin, end,
		H(0), //Initial seed
		hashCombine<typename std::iterator_traits<InputIt>::value_type, H>
	);
}

}