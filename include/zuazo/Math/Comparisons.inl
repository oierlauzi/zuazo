#include "Interpolation.h"

#include "Vector.h"

#include <algorithm>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept {
    return std::min(a, b);
}

template<typename T, size_t N>
constexpr Vec<T, N> min(const Vec<T, N>& a, const Vec<T, N>& b) {
    return transform(
        [] (const auto& a, const auto& b) -> auto {
            return min(a, b);
        },
        a, b
    );
}


template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept {
    return std::max(a, b);
}

template<typename T, size_t N>
constexpr Vec<T, N> max(const Vec<T, N>& a, const Vec<T, N>& b) {
    return transform(
        [] (const auto& a, const auto& b) -> auto {
            return max(a, b);
        },
        a, b
    );
}


template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept {
	return Math::min(hi, Math::max(lo, val));
}

template<typename T, size_t N>
constexpr Vec<T, N> clamp(const Vec<T, N>& val, const Vec<T, N>& lo, const Vec<T, N>& hi) {
	return Math::min(hi, Math::max(lo, val));
}


template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept {
    return lo <= val && val <= hi;
}

template<typename T>
constexpr bool isInRangeExclusive(const T& val, const T& lo, const T& hi) noexcept {
    return lo < val && val < hi;
}
 
}