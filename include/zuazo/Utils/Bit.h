#pragma once

#include <cstddef>

namespace Zuazo::Utils {

template<typename T>
constexpr T bit(T pos) noexcept;

template<typename T>
constexpr size_t flagCount(const T& bitset) noexcept;

}

#include "Bit.inl"