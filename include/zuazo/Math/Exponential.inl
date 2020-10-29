#include "Exponential.h"

#include <cmath>

namespace Zuazo::Math {

inline float exp(float x) noexcept {
	return std::exp(x);
}

inline double exp(double x) noexcept {
	return std::exp(x);
}


inline float exp2(float x) noexcept {
	return std::exp2(x);
}

inline double exp2(double x) noexcept {
	return std::exp2(x);
}


inline float exp10(float x) noexcept {
	return Math::pow(10.0f, x);
}

inline double exp10(double x) noexcept {
	return Math::pow(10.0, x);
}



inline float log(float x) noexcept {
	return std::log(x);
}

inline double log(double x) noexcept {
	return std::log(x);
}


inline float log2(float x) noexcept {
	return std::log2(x);
}

inline double log2(double x) noexcept {
	return std::log2(x);
}


inline float log10(float x) noexcept {
	return std::log10(x);
}

inline double log10(double x) noexcept {
	return std::log10(x);
}


inline float pow(float base, float power) noexcept {
	return std::pow(base, power);
}

inline double pow(double base, double power) noexcept {
	return std::pow(base, power);
}


inline float sqrt(float x) noexcept {
	return std::sqrt(x);
}

inline double sqrt(double x) noexcept {
	return std::sqrt(x);
}

}