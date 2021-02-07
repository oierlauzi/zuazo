#include "Rounding.h"

#include "Rational.h"
#include "Vector.h"

#include <cmath>

namespace Zuazo::Math {

inline float floor(float x) noexcept {
    return std::floor(x);
}

inline double floor(double x) noexcept {
    return std::floor(x);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> floor(const Rational<num_t, den_t>& r) {
	const auto almostOne = Rational<num_t, den_t>(r.getDenominator() - 1, r.getDenominator());
	return (r.getNumerator() < 0) ? -trunc(-r + almostOne) : trunc(r);
}

template<typename T, size_t N>
constexpr Vec<T, N> floor(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return floor(x);
		},
		v
	);
}



inline float trunc(float x) noexcept {
	return std::trunc(x);
}

inline double trunc(double x) noexcept {
	return std::trunc(x);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> trunc(const Rational<num_t, den_t>& r) {
	return Rational<num_t, den_t>(static_cast<typename Rational<num_t, den_t>::Num>(r));
}

template<typename T, size_t N>
constexpr Vec<T, N> trunc(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return trunc(x);
		},
		v
	);
}



inline float ceil(float x) noexcept {
	return std::ceil(x);
}

inline double ceil(double x) noexcept {
	return std::ceil(x);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> ceil(const Rational<num_t, den_t>& r) {
	const auto almostOne = Rational<num_t, den_t>(r.getDenominator() - 1, r.getDenominator());
	return (r.getNumerator() < 0) ? -trunc(-r) : trunc(r + almostOne);
}

template<typename T, size_t N>
constexpr Vec<T, N> ceil(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return ceil(x);
		},
		v
	);
}



inline float round(float x) noexcept {
	return std::round(x);
}

inline double round(double x) noexcept {
	return std::round(x);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> round(const Rational<num_t, den_t>& r) {
	return trunc(r + Rational<num_t, den_t>(1, 2));
}

template<typename T, size_t N>
constexpr Vec<T, N> round(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return round(x);
		},
		v
	);
}



inline float fract(float x) noexcept {
	return x - trunc(x);
}

inline double fract(double x) noexcept {
	return x - trunc(x);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> fract(const Rational<num_t, den_t>& r) {
	return Rational<num_t, den_t>(r.getNumerator() % r.getDenominator(), r.getDenominator());
}

template<typename T, size_t N>
constexpr Vec<T, N> fract(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return fract(x);
		},
		v
	);
}

}