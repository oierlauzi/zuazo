#pragma once

#include "Vector.h"

namespace Zuazo::Math {

template<typename T, size_t N>
class Vec;


float exp(float x) noexcept;
double exp(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> exp(const Vec<T, N>& v);


float exp2(float x) noexcept;
double exp2(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> exp2(const Vec<T, N>& v);


float exp10(float x) noexcept;
double exp10(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> exp10(const Vec<T, N>& v);


float log(float x) noexcept;
double log(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> log(const Vec<T, N>& v);


float log2(float x) noexcept;
double log2(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> log2(const Vec<T, N>& v);


float log10(float x) noexcept;
double log10(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> log10(const Vec<T, N>& v);


float pow(float base, float power) noexcept;
double pow(double base, double power) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const Vec<T, N>& power);

template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const typename Vec<T, N>::value_type& power);

template<typename T, size_t N>
constexpr Vec<T, N> pow(const typename Vec<T, N>::value_type& base, const Vec<T, N>& power);


float sqrt(float x) noexcept;
double sqrt(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> sqrt(const Vec<T, N>& v);


float cbrt(float x) noexcept;
double cbrt(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> cbrt(const Vec<T, N>& v);

}

#include "Exponential.inl"