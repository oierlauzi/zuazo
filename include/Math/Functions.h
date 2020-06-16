#pragma once

namespace Zuazo::Math {
	
template<typename T>
constexpr const T& min(const T& a, const T& b);

template<typename T, typename... Q>
constexpr const T& min(const T& first, const Q&... rest);

template<typename T>
constexpr const T& max(const T& a, const T& b);

template<typename T, typename... Q>
constexpr const T& max(const T& first, const Q&... rest);

template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi);

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r);

template<typename T>
constexpr T ilerp(const T& a, const T& b, const T& val);

template<typename T>
constexpr T map(const T& value, const T& lo1, const T& hi1, const T& lo2, const T& hi2);

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi);

}

#include "Functions.inl"