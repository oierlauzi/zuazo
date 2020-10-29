#pragma once

namespace Zuazo::Math {

template<typename T>
constexpr T mod(const T& num, const T& den) noexcept;
float mod(float num, float den) noexcept;
double mod(double num, double den) noexcept;

template<typename T>
constexpr T gcd(const T& a, const T& b) noexcept;

template<typename T>
constexpr T lcm(const T& a, const T& b) noexcept;

}

#include "Factor.inl"