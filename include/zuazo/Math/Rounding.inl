#include "Rounding.h"

#include <cmath>

namespace Zuazo::Math {

inline float floor(float x) noexcept {
    return std::floor(x);
}

inline double floor(double x) noexcept {
    return std::floor(x);
}


inline float trunc(float x) noexcept {
	return std::trunc(x);
}

inline double trunc(double x) noexcept {
	return std::trunc(x);
}


inline float ceil(float x) noexcept {
	return std::ceil(x);
}

inline double ceil(double x) noexcept {
	return std::ceil(x);
}


inline float round(float x) noexcept {
	return std::round(x);
}

inline double round(double x) noexcept {
	return std::round(x);
}


inline float fract(float x) noexcept {
	return x - trunc(x);
}

inline double fract(double x) noexcept {
	return x - trunc(x);
}

}