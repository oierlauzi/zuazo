#include "Interpolation.h"

namespace Zuazo::Math {

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept {
	return static_cast<T>(a + (b - a) * r);
}

template<typename T, typename Q>
constexpr Q ilerp(const T& a, const T& b, const T& val) noexcept {
	return static_cast<Q>(val - a) / static_cast<Q>(b - a);
}

}