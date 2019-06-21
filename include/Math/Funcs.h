#pragma once

namespace Zuazo::Math {

template<typename T>
constexpr T abs(T n);

template<typename T>
constexpr T gcd(T a, T b);

template<typename T>
constexpr T lcm(T a, T b);

}

#include "Funcs.inl"