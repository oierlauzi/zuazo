#pragma once

namespace Zuazo::Math {

template<typename num_t, typename den_t>
class Rational;

template<typename T, size_t N>
class Vec;


template<typename T>
constexpr T mod(const T& num, const T& den);
float mod(float num, float den) noexcept;
double mod(double num, double den) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> mod(const Rational<num_t, den_t>& num, const Rational<num_t, den_t>& den);


template<typename T>
constexpr T gcd(const T& a, const T& b);

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> gcd(const Rational<num_t, den_t>& a, const Rational<num_t, den_t>& b);

template<typename T>
constexpr T lcm(const T& a, const T& b);

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> lcm(const Rational<num_t, den_t>& a, const Rational<num_t, den_t>& b);

}

#include "Factor.inl"