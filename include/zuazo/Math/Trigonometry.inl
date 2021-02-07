#include "Trigonometry.h"

#include "Vector.h"

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

template<typename T, size_t N>
constexpr Vec<T, N> cos(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return cos(x);
		},
		v
	);
}


inline float cosh(float x) noexcept {
	return std::cosh(x);
}

inline double cosh(double x) noexcept {
	return std::cosh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> cosh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return cosh(x);
		},
		v
	);
}


inline float sin(float x) noexcept {
	return std::sin(x);
}

inline double sin(double x) noexcept {
	return std::sin(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> sin(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return sin(x);
		},
		v
	);
}


inline float sinh(float x) noexcept {
	return std::sinh(x);
}

inline double sinh(double x) noexcept {
	return std::sinh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> sinh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return sinh(x);
		},
		v
	);
}


inline float tan(float x) noexcept {
	return std::tan(x);
}

inline double tan(double x) noexcept {
	return std::tan(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> tan(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return tan(x);
		},
		v
	);
}


inline float tanh(float x) noexcept {
	return std::tanh(x);
}

inline double tanh(double x) noexcept {
	return std::tanh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> tanh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return tanh(x);
		},
		v
	);
}


inline float acos(float x) noexcept {
	return std::acos(x);
}

inline double acos(double x) noexcept {
	return std::acos(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> acos(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return acos(x);
		},
		v
	);
}


inline float acosh(float x) noexcept {
	return std::acosh(x);
}

inline double acosh(double x) noexcept {
	return std::acosh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> acosh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return acosh(x);
		},
		v
	);
}


inline float asin(float x) noexcept {
	return std::asin(x);
}

inline double asin(double x) noexcept {
	return std::asin(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> asin(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return asin(x);
		},
		v
	);
}


inline float asinh(float x) noexcept {
	return std::asinh(x);
}

inline double asinh(double x) noexcept {
	return std::asinh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> asinh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return asinh(x);
		},
		v
	);
}


inline float atan(float x) noexcept {
	return std::atan(x);
}

inline float atan(float y, float x) noexcept {
	return std::atan2(y, x);	
}

inline double atan(double x) noexcept {
	return std::atan(x);
}

inline double atan(double y, double x) noexcept {
	return std::atan2(y, x);	
}

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return atan(x);
		},
		v
	);
}

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& vy, const Vec<T, N>& vx) {
	return tansform(
		[] (const auto& y, const auto& x) -> auto {
			return atan(y, x);
		},
		vy, vx
	);
}


inline float atanh(float x) noexcept {
	return std::atanh(x);
}

inline double atanh(double x) noexcept {
	return std::atanh(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> atanh(const Vec<T, N>& v) {
	return tansform(
		[] (const auto& x) -> auto {
			return atanh(x);
		},
		v
	);
}

}