#include "Functions.h"

namespace Zuazo::Utils {

template<typename T>
constexpr T bit(T pos){
	return T(1) << pos;
}

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment) {
	if(ptr % alignment) {
		return (ptr / alignment + 1) * alignment;
	} else {
		return ptr;
	}
}

}