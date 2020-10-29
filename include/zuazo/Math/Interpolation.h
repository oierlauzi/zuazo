#pragma once

namespace Zuazo::Math {

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept;

template<typename T, typename Q = T>
constexpr Q ilerp(const T& val, const T& a, const T& b) noexcept;

template<typename T, typename Q = T>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2) noexcept;
    
}

#include "Interpolation.inl"