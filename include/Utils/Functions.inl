#include "Functions.h"

#include <algorithm>
#include <cassert>
#include <tuple>
#include <functional>

namespace Zuazo::Utils {

template<typename... T>
constexpr void ignore(const T&...){

}

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

namespace Zuazo {
	
inline std::string toString(uint8_t x) {
	return std::to_string(x);
}

inline std::string toString(int8_t x) {
	return std::to_string(x);
}

inline std::string toString(uint16_t x) {
	return std::to_string(x);
}

inline std::string toString(int16_t x) {
	return std::to_string(x);
}

inline std::string toString(uint32_t x) {
	return std::to_string(x);
}

inline std::string toString(int32_t x) {
	return std::to_string(x);
}

inline std::string toString(uint64_t x) {
	return std::to_string(x);
}

inline std::string toString(int64_t x) {
	return std::to_string(x);
}

inline std::string toString(float x) {
	return std::to_string(x);
}

inline std::string toString(double x) {
	return std::to_string(x);
}

}