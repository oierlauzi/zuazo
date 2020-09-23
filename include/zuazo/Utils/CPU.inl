#include "CPU.h"

#include <utility>

namespace Zuazo::Utils {

constexpr uintptr_t align(uintptr_t ptr, size_t alignment) {
	return ((ptr + alignment - 1) / alignment) * alignment;
}

template<typename T>
inline T* align(T* ptr, size_t alignment) {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
}



constexpr bool isBigEndian() {
	//Based on rapidjson
	//Detect with GCC 4.6's macro.
	#if defined(__BYTE_ORDER__)
		#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
			return true;
		#else
			return false;
		#endif

    // Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro.
	#elif defined(_LITTLE_ENDIAN) || defined(_BIG_ENDIAN)
		#if defined(_BIG_ENDIAN)
			return true;
		#else
			return false;
		#endif

	#else
		static_assert(false, "Could not deduce endianess");
		return false;

	#endif
}

constexpr bool isLittleEndian() {
	//Based on rapidjson
	//Detect with GCC 4.6's macro.
	#if defined(__BYTE_ORDER__)
		#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
			return true;
		#else
			return false;
		#endif

    // Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro.
	#elif defined(_LITTLE_ENDIAN) || defined(_BIG_ENDIAN)
		#if defined(_LITTLE_ENDIAN)
			return true;
		#else
			return false;
		#endif

	#else
		static_assert(false, "Could not deduce endianess");
		return false;

	#endif
}

static_assert(isBigEndian() == !isLittleEndian(), "Unknown endianess");


template<typename T>
constexpr T& bele(T& be, T& le) {
	return isBigEndian() ? be : le;
}

template<typename T>
constexpr const T& bele(const T& be, const T& le) {
	return isBigEndian() ? be : le; 
}

constexpr void swapEndianess(std::byte* begin, std::byte* end) {
	end--;
	while(begin < end) {
		std::swap(*begin, *end);
		begin++;
		end--;
	}
}

template<typename T>
inline typename std::enable_if<std::is_scalar<T>::value, T>::type swapEndianess(T x) {
	swapEndianess(
		reinterpret_cast<std::byte*>(&x),
		reinterpret_cast<std::byte*>(&x + 1)
	);
	return x;
}

}
