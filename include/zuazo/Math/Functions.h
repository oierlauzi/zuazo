#pragma once

#include <type_traits>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b);

template<typename T>
constexpr const T& min(std::initializer_list<T> ilist);

template<typename T>
constexpr const T& max(const T& a, const T& b);

template<typename T>
constexpr const T& max(std::initializer_list<T> ilist);

template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi);


template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r);

template<typename T, typename Q = double>
constexpr Q ilerp(const T& val, const T& a, const T& b);

template<typename T, typename Q = double>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2);


template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi);

template<typename T>
constexpr T abs(const T& val);

template<typename T>
constexpr T inv(const T& val);

template<typename T>
constexpr T mod(const T& num, const T& den);

template<typename T>
T floor(T x);
template<typename T>
T trunc(T x);
template<typename T>
T ceil(T x);
template<typename T>
T round(T x);
template<typename T>
T frac(T x);

template<typename T>
constexpr T gcd(const T& a, const T& b);

template<typename T>
constexpr T lcm(const T& a, const T& b);

}

#include "Functions.inl"