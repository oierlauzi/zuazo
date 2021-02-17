#include "Exponential.h"

#include "Vector.h"

#include <cmath>

namespace Zuazo::Math {

inline float exp(float x) noexcept {
	return std::exp(x);
}

inline double exp(double x) noexcept {
	return std::exp(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> exp(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return exp(x);
		},
		v
	);
}


inline float exp2(float x) noexcept {
	return std::exp2(x);
}

inline double exp2(double x) noexcept {
	return std::exp2(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> exp2(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return exp2(x);
		},
		v
	);
}


inline float exp10(float x) noexcept {
	return Math::pow(10.0f, x);
}

inline double exp10(double x) noexcept {
	return Math::pow(10.0, x);
}

template<typename T, size_t N>
constexpr Vec<T, N> exp10(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return exp10(x);
		},
		v
	);
}


inline float log(float x) noexcept {
	return std::log(x);
}

inline double log(double x) noexcept {
	return std::log(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> log(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return log(x);
		},
		v
	);
}


inline float log2(float x) noexcept {
	return std::log2(x);
}

inline double log2(double x) noexcept {
	return std::log2(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> log2(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return log2(x);
		},
		v
	);
}


inline float log10(float x) noexcept {
	return std::log10(x);
}

inline double log10(double x) noexcept {
	return std::log10(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> log10(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return log10(x);
		},
		v
	);
}


inline float pow(float base, float power) noexcept {
	return std::pow(base, power);
}

inline double pow(double base, double power) noexcept {
	return std::pow(base, power);
}

template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const Vec<T, N>& power) {
	return transform(
		[] (const auto& base, const auto& power) -> auto {
			return pow(base, power);
		},
		base, power
	);
}

template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const typename Vec<T, N>::value_type& power) {
	return transform(
		[&power] (const auto& base) -> auto {
			return pow(base, power);
		},
		base
	);
}

template<typename T, size_t N>
constexpr Vec<T, N> pow(const typename Vec<T, N>::value_type& base, const Vec<T, N>& power) {
	return transform(
		[&base] (const auto& power) -> auto {
			return pow(base, power);
		},
		power
	);
}


inline float sqrt(float x) noexcept {
	return std::sqrt(x);
}

inline double sqrt(double x) noexcept {
	return std::sqrt(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> sqrt(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return sqrt(x);
		},
		v
	);
}


inline float cbrt(float x) noexcept {
	return std::cbrt(x);
}

inline double cbrt(double x) noexcept {
	return std::cbrt(x);
}

template<typename T, size_t N>
constexpr Vec<T, N> cbrt(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return cbrt(x);
		},
		v
	);
}

}