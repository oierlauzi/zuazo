#pragma once

namespace Zuazo::Math {

template<typename T, size_t N>
class Vec;


template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> min(const Vec<T, N>& a, const Vec<T, N>& b);


template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> max(const Vec<T, N>& a, const Vec<T, N>& b);


template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> clamp(const Vec<T, N>& val, const Vec<T, N>& lo, const Vec<T, N>& hi);


template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept;

template<typename T>
constexpr bool isInRangeExclusive(const T& val, const T& lo, const T& hi) noexcept;

}

#include "Comparisons.inl"