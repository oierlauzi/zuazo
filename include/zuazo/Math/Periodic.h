#pragma once

namespace Zuazo::Math {

template<typename T>
constexpr T sawtooth(const T& period, const T& value) noexcept;

template<typename T, typename R>
constexpr R sawtooth(const T& period, const R& amplitude, const T& value) noexcept;

template<typename T>
constexpr T triangle(const T& period, const T& value) noexcept;

template<typename T, typename R>
constexpr R triangle(const T& period, const R& amplitude, const T& value) noexcept;

template<typename T>
constexpr T pulse(const T& period, const T& uptime, const T& value) noexcept;

template<typename T, typename R>
constexpr R pulse(const T& period, const T& uptime, const R& amplitude, const T& value) noexcept;

}

#include "Periodic.inl"