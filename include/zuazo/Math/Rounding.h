#pragma once

namespace Zuazo::Math {

template<typename num_t, typename den_t>
class Rational;

template<typename T, size_t N>
class Vec;


float floor(float x) noexcept;
double floor(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> floor(const Rational<num_t, den_t>& r);

template<typename T, size_t N>
constexpr Vec<T, N> floor(const Vec<T, N>& v);


float trunc(float x) noexcept;
double trunc(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> trunc(const Rational<num_t, den_t>& r);

template<typename T, size_t N>
constexpr Vec<T, N> trunc(const Vec<T, N>& v);



float ceil(float x) noexcept;
double ceil(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> ceil(const Rational<num_t, den_t>& r);

template<typename T, size_t N>
constexpr Vec<T, N> ceil(const Vec<T, N>& v);



float round(float x) noexcept;
double round(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> round(const Rational<num_t, den_t>& r);

template<typename T, size_t N>
constexpr Vec<T, N> round(const Vec<T, N>& v);



float fract(float x) noexcept;
double fract(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> fract(const Rational<num_t, den_t>& r);

template<typename T, size_t N>
constexpr Vec<T, N> fract(const Vec<T, N>& v);


}

#include "Rounding.inl"