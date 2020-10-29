#pragma once

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept;

template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept;

template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept;

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept;

}

#include "Comparisons.inl"