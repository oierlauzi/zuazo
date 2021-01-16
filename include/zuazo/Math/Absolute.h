#pragma once

namespace Zuazo::Math {

template<typename T>
constexpr T abs(const T& x) noexcept;
float abs(float x) noexcept;
double abs(double x) noexcept;

}

#include "Absolute.inl"