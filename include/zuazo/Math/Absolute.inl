#include "Absolute.h"

#include <cmath>

namespace Zuazo::Math {

template<typename T>
constexpr T abs(const T& x) noexcept {
	return (x < T()) ? -x : x;
}

inline float abs(float x) noexcept {
	return std::abs(x);	
}

inline double abs(double x) noexcept {
	return std::abs(x);	
}

}