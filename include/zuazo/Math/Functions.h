#pragma once

#include <type_traits>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept;

template<typename T>
constexpr const T& min(std::initializer_list<T> ilist) noexcept;

template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept;

template<typename T>
constexpr const T& max(std::initializer_list<T> ilist) noexcept;

template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept;


template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept;

template<typename T, typename Q = double>
constexpr Q ilerp(const T& val, const T& a, const T& b) noexcept;

template<typename T, typename Q = double>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2) noexcept;


template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept;

template<typename T>
constexpr T abs(const T& val) noexcept;

template<typename T>
constexpr T inv(const T& val) noexcept;

template<typename T>
constexpr T mod(const T& num, const T& den) noexcept;

template<typename T>
T floor(T x) noexcept;
template<typename T>
T trunc(T x) noexcept;
template<typename T>
T ceil(T x) noexcept;
template<typename T>
T round(T x) noexcept;
template<typename T>
T frac(T x) noexcept;

template<typename T>
constexpr T gcd(const T& a, const T& b) noexcept;

template<typename T>
constexpr T lcm(const T& a, const T& b) noexcept;



template<typename T>
constexpr T sawtooth(const T& period, const T& value) noexcept;

template<typename T, typename R>
constexpr R sawtooth(const T& period, const R& amplitude, const T& value) noexcept;

template<typename T>
constexpr T triangle(const T& period, const T& value) noexcept;

template<typename T, typename R>
constexpr R triangle(const T& period, const R& amplitude, const T& value) noexcept;

}

#include "Functions.inl"