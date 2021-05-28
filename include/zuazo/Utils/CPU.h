#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace Zuazo::Utils {

enum class Endianess {
	little,
	big,
	unknown
};

constexpr Endianess getEndianess() noexcept;

template<typename T>
constexpr T& bele(T& be, T& le) noexcept;

template<typename T>
constexpr const T& bele(const T& be, const T& le) noexcept;

constexpr void swapEndianess(std::byte* begin, std::byte* end) noexcept;

template<typename T>
typename std::enable_if<std::is_scalar<T>::value, T>::type swapEndianess(T x) noexcept;



constexpr size_t getByteSize() noexcept;



constexpr uintptr_t align(uintptr_t ptr, size_t alignment) noexcept;

template<typename T>
T* align(T* ptr, size_t alignment) noexcept;


}

#include "CPU.inl"
