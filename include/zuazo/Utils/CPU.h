#pragma once

#include <cstdint>
#include <cstddef>
#include <climits>
#include <type_traits>

namespace Zuazo::Utils {

constexpr size_t BYTE_SIZE = CHAR_BIT;

constexpr uintptr_t align(uintptr_t ptr, size_t alignment);

template<typename T>
T* align(T* ptr, size_t alignment);

constexpr bool isBigEndian();
constexpr bool isLittleEndian();

template<typename T>
constexpr T& bele(T& be, T& le);

template<typename T>
constexpr const T& bele(const T& be, const T& le);

constexpr void swapEndianess(std::byte* begin, std::byte* end);

template<typename T>
typename std::enable_if<std::is_scalar<T>::value, T>::type swapEndianess(T x);

}

#include "CPU.inl"