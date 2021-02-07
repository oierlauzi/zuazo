#pragma once

#include <cstdint>

namespace Zuazo::Math {

template<typename num_t, typename den_t>
class Rational;

template<typename T, size_t N>
class Vec;


template<typename T>
constexpr T abs(const T& x);

constexpr int8_t abs(int8_t x);
constexpr uint8_t abs(uint8_t x);
constexpr int16_t abs(int16_t x);
constexpr uint16_t abs(uint16_t x);
constexpr int32_t abs(int32_t x);
constexpr uint32_t abs(uint32_t x);
constexpr int64_t abs(int64_t x);
constexpr uint64_t abs(uint64_t x);
float abs(float x) noexcept;
double abs(double x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> abs(const Rational<num_t, den_t>& r) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> abs(const Vec<T, N>& v);


}

#include "Absolute.inl"