#pragma once

namespace Zuazo::Math {

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept;

template<typename T, typename Q = T>
constexpr Q ilerp(const T& a, const T& b, const T& val) noexcept;
    
}

#include "Interpolation.inl"