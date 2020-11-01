#include "Bit.h"

#include "CPU.h"

namespace Zuazo::Utils {

template<typename T>
constexpr T bit(T pos) noexcept {
	return T(1) << pos;
}

template<typename T>
constexpr size_t flagCount(const T& bitset) noexcept {
	constexpr auto BIT_COUNT = sizeof(T) * getByteSize();
	size_t cnt = 0;

	for(size_t i = 0; i < BIT_COUNT; ++i) {
		if(bitset & static_cast<T>(bit(i))) {
			++cnt;
		}
	}

	return cnt;
}

}
