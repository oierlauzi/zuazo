#include "Trigonometry.h"

#include <cmath>

namespace Zuazo::Math {

template <typename T>
constexpr T rad2deg(const T& radians) noexcept {
	return radians * (T(180) / M_PI);
}

template <typename T>
constexpr T deg2rad(const T& degrees) noexcept {
	return degrees * (M_PI / T(180));
}



inline float cos(float x) noexcept {
	return std::cos(x);
}

inline double cos(double x) noexcept {
	return std::cos(x);
}


inline float cosh(float x) noexcept {
	return std::cosh(x);
}

inline double cosh(double x) noexcept {
	return std::cosh(x);
}


inline float sin(float x) noexcept {
	return std::sin(x);
}

inline double sin(double x) noexcept {
	return std::sin(x);
}


inline float sinh(float x) noexcept {
	return std::sinh(x);
}

inline double sinh(double x) noexcept {
	return std::sinh(x);
}


inline float tan(float x) noexcept {
	return std::tan(x);
}

inline double tan(double x) noexcept {
	return std::tan(x);
}


inline float tanh(float x) noexcept {
	return std::tanh(x);
}

inline double tanh(double x) noexcept {
	return std::tanh(x);
}



inline float acos(float x) noexcept {
	return std::acos(x);
}

inline double acos(double x) noexcept {
	return std::acos(x);
}


inline float acosh(float x) noexcept {
	return std::acosh(x);
}

inline double acosh(double x) noexcept {
	return std::acosh(x);
}


inline float asin(float x) noexcept {
	return std::asin(x);
}

inline double asin(double x) noexcept {
	return std::asin(x);
}


inline float asinh(float x) noexcept {
	return std::asinh(x);
}

inline double asinh(double x) noexcept {
	return std::asinh(x);
}


inline float atan(float x) noexcept {
	return std::atan(x);
}

inline double atan(double x) noexcept {
	return std::atan(x);
}


inline float atan(float y, float x) noexcept {
	return std::atan2(y, x);	
}

inline double atan(double y, double x) noexcept {
	return std::atan2(y, x);	
}


inline float atanh(float x) noexcept {
	return std::atanh(x);
}

inline double atanh(double x) noexcept {
	return std::atanh(x);
}

}