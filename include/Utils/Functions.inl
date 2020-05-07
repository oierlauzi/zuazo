#include "Functions.h"

#include <algorithm>
#include <cassert>
#include <tuple>
#include <functional>

namespace Zuazo::Utils {

template<typename... T>
constexpr void ignore(const T&...){

}

template<typename T>
constexpr T bit(T pos){
	return T(1) << pos;
}

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment) {
	if(ptr % alignment) {
		return (ptr / alignment + 1) * alignment;
	} else {
		return ptr;
	}
}




template<typename T>
constexpr const T& min(const T& a, const T& b) {
	return std::min(a, b);
}

template<typename T>
constexpr T min(std::initializer_list<T> ilist) {
	assert(ilist.size() > 0);
	auto result = *ilist.begin();

	for(auto ite = ilist.begin() + 1; ite != ilist.end(); ++ite){
		result = min(result, *ite);
	}

	return result;
}

template<typename T>
constexpr const T& max(const T& a, const T& b) {
	return std::max(a, b);
}

template<typename T>
constexpr T max(std::initializer_list<T> ilist) {
	assert(ilist.size() > 0);
	auto result = *ilist.begin();

	for(auto ite = ilist.begin() + 1; ite != ilist.end(); ++ite){
		result = max(result, *ite);
	}

	return result;
}


template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) {
	return min(hi, max(lo, val));
}

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) {
	return (lo <= val) && (val <= hi);
}



namespace details {

template<typename Func, typename Tuples, size_t I>
constexpr auto elementwiseOperationImpl2(Func&& op, Tuples&& tuples, std::index_sequence<I>) {
    return std::apply(
        [&] (auto&&... x) -> auto {
            return op(std::get<I>(std::forward<decltype(x)>(x))...);
        },
       std::forward<Tuples>(tuples)
    );
}

template<typename Func, typename Tuples, size_t... I>
constexpr auto elementwiseOperationImpl1(Func&& op, Tuples&& tuples, std::index_sequence<I...>) {
	return std::make_tuple(
        elementWiseOperationImpl2( 
            std::forward<Func>(op), 
            std::forward<Tuples>(tuples),
            std::index_sequence<I>()
        )...
    );
}

}

template<typename Func, typename Tuple1, typename... Tuplen>
constexpr auto elementwiseOperation(Func&& op, Tuple1&& first, Tuplen&&... rest) {
	constexpr auto ARG_COUNT = std::tuple_size<typename std::remove_reference<Tuple1>::type>::value; //Also valid for pair and array
	
    return details::elementwiseOperationImpl1(
		std::forward<Func>(op),
		std::forward_as_tuple<Tuple1, Tuplen...>(first, rest...),
        std::make_index_sequence<ARG_COUNT>()
    );
}

}

namespace Zuazo {
	
inline std::string toString(uint8_t x) {
	return std::to_string(x);
}

inline std::string toString(int8_t x) {
	return std::to_string(x);
}

inline std::string toString(uint16_t x) {
	return std::to_string(x);
}

inline std::string toString(int16_t x) {
	return std::to_string(x);
}

inline std::string toString(uint32_t x) {
	return std::to_string(x);
}

inline std::string toString(int32_t x) {
	return std::to_string(x);
}

inline std::string toString(uint64_t x) {
	return std::to_string(x);
}

inline std::string toString(int64_t x) {
	return std::to_string(x);
}

inline std::string toString(float x) {
	return std::to_string(x);
}

inline std::string toString(double x) {
	return std::to_string(x);
}

}