#include "Functions.h"

#include <functional>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) {
	return std::min(a, b);
}

template<typename T, typename... Q>
constexpr const T& min(const T& first, const Q&... rest) {
	return min(first, min(rest...));
}


template<typename T>
constexpr const T& max(const T& a, const T& b) {
	return std::max(a, b);
}

template<typename T, typename... Q>
constexpr const T& max(const T& first, const Q&... rest) {
	return max(first, max(rest...));
}



template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) {
	return min(hi, max(lo, val));
}

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) {
	return static_cast<T>(a + (b - a) * r);
}

template<typename T>
constexpr T ilerp(const T& a, const T& b, const T& val) {
	return (val - a) / (b - a);
}

template<typename T>
constexpr T map(const T& value, const T& lo1, const T& hi1, const T& lo2, const T& hi2) {
	return lerp(lo2, hi2, ilerp(lo1, hi1, value));
}

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) {
	return (lo <= val) && (val <= hi);
}

}