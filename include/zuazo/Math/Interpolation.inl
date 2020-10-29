#include "Interpolation.h"

namespace Zuazo::Math {

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept {
	return static_cast<T>(a + (b - a) * r);
}

template<typename T, typename Q>
constexpr Q ilerp(const T& val, const T& a, const T& b) noexcept {
	return static_cast<Q>(val - a) / static_cast<Q>(b - a);
}

template<typename T, typename Q>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2) noexcept {
	return Math::lerp(lo2, hi2, Math::ilerp<T, Q>(val, lo1, hi1));
}

}