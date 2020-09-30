#include "CPU.h"

#include <utility>
#include <climits>

namespace Zuazo::Utils {

constexpr Endianess getEndianess() {
	//Based on rapidjson
	//Detect with GCC 4.6's macro.
	#if defined(__BYTE_ORDER__)
		#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
			return Endianess::LITTLE;
		#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
			return Endianess::BIG;
		#else
			return Endianess::UNKNOWN;
		#endif

    // Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro.
	#elif defined(_LITTLE_ENDIAN) || defined(_BIG_ENDIAN)
		#if defined(_LITTLE_ENDIAN)
			return Endianess::LITTLE;
		#elif defined(_BIG_ENDIAN)
			return Endianess::BIG;
		#else
			return Endianess::UNKNOWN;
		#endif

	#else
		return Endianess::UNKNOWN;
	#endif
}

template<typename T>
constexpr T& bele(T& be, T& le) {
	constexpr auto endianess = getEndianess();
	static_assert(endianess == Endianess::LITTLE || endianess == Endianess::BIG, "Endianess must be LE or BE");
	return endianess == Endianess::BIG ? be : le;
}

template<typename T>
constexpr const T& bele(const T& be, const T& le) {
	constexpr auto endianess = getEndianess();
	static_assert(endianess == Endianess::LITTLE || endianess == Endianess::BIG, "Endianess must be LE or BE");
	return endianess == Endianess::BIG ? be : le;
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



constexpr size_t getByteSize() {
	return CHAR_BIT; //Usually 8
}



constexpr uintptr_t align(uintptr_t ptr, size_t alignment) {
	return ((ptr + alignment - 1) / alignment) * alignment;
}

template<typename T>
inline T* align(T* ptr, size_t alignment) {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
}

}
