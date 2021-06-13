#include "CPU.h"

#include <utility>
#include <climits>

namespace Zuazo::Utils {

constexpr Endianess getEndianess() noexcept {
	//Based on rapidjson
	//Detect with GCC 4.6's macro.
	#if defined(__BYTE_ORDER__)
		#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
			return Endianess::little;
		#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
			return Endianess::big;
		#else
			return Endianess::unknown;
		#endif

    // Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro.
	#elif defined(_LITTLE_ENDIAN) || defined(_BIG_ENDIAN)
		#if defined(_LITTLE_ENDIAN)
			return Endianess::little;
		#elif defined(_BIG_ENDIAN)
			return Endianess::big;
		#else
			return Endianess::unknown;
		#endif

	#else
		return Endianess::unknown;
	#endif
}

template<typename T>
constexpr T& bele(T& be, T& le) noexcept {
	constexpr auto endianess = getEndianess();
	static_assert(endianess == Endianess::little || endianess == Endianess::big, "Endianess must be LE or BE");
	return endianess == Endianess::big ? be : le;
}

template<typename T>
constexpr const T& bele(const T& be, const T& le) noexcept {
	constexpr auto endianess = getEndianess();
	static_assert(endianess == Endianess::little || endianess == Endianess::big, "Endianess must be LE or BE");
	return endianess == Endianess::big ? be : le;
}

constexpr void swapEndianess(std::byte* begin, std::byte* end) noexcept {
	end--;
	while(begin < end) {
		std::swap(*begin, *end);
		begin++;
		end--;
	}
}

template<typename T>
inline typename std::enable_if<std::is_scalar<T>::value, T>::type swapEndianess(T x) noexcept {
	swapEndianess(
		reinterpret_cast<std::byte*>(&x),
		reinterpret_cast<std::byte*>(&x + 1)
	);
	return x;
}



constexpr size_t getByteSize() noexcept {
	return CHAR_BIT; //Usually 8
}



constexpr uintptr_t alignLower(uintptr_t ptr, size_t alignment) noexcept {
	return (ptr / alignment) * alignment;
}

constexpr uintptr_t alignUpper(uintptr_t ptr, size_t alignment) noexcept {
	return ((ptr + alignment - 1) / alignment) * alignment;
}

constexpr uintptr_t align(uintptr_t ptr, size_t alignment) noexcept {
	return alignUpper(ptr, alignment);
}


template<typename T>
inline T* alignLower(T* ptr, size_t alignment) noexcept {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(alignLower(reinterpret_cast<uintptr_t>(ptr), alignment));
}

template<typename T>
inline T* alignUpper(T* ptr, size_t alignment) noexcept {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(alignUpper(reinterpret_cast<uintptr_t>(ptr), alignment));
}

template<typename T>
inline T* align(T* ptr, size_t alignment) noexcept {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
}

}
