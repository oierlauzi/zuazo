#pragma once

#include <array>
#include <cstddef>

namespace Zuazo::Math {

template<typename T>
constexpr T factorial(const T& num) noexcept;

template<typename T>
constexpr T binomialCoefficient(const T& n, const T& k) noexcept;

template<typename T, size_t N>
constexpr std::array<T, N+1> generateBinomialCoefficients() noexcept;

}

#include "Combinatorics.inl"