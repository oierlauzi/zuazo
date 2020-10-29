#include "Interpolation.h"

#include <algorithm>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept {
    return std::min(a, b);
}

template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept {
    return std::max(a, b);
}

template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept {
	return Math::min(hi, Math::max(lo, val));
}

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept {
    return lo <= val && val <= hi;
}
 
}