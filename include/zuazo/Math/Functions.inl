#include "Functions.h"

#include <functional>
#include <cmath>
#include <cassert>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) noexcept {
	return std::min(a, b);
}

template<typename T>
constexpr const T& min(std::initializer_list<T> ilist) noexcept {
	return std::min(ilist);
}


template<typename T>
constexpr const T& max(const T& a, const T& b) noexcept {
	return std::max(a, b);
}

template<typename T>
constexpr const T& max(std::initializer_list<T> ilist) noexcept {
	return std::max(ilist);
}



template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) noexcept {
	return Math::min(hi, Math::max(lo, val));
}

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) noexcept {
	return static_cast<T>(a + (b - a) * r);
}

template<typename T, typename Q>
constexpr Q ilerp(const T& val, const T& a, const T& b) noexcept {
	return static_cast<Q>(val - a) / static_cast<Q>(b - a);
}

template<typename T, typename Q>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2) noexcept {
	return Math::lerp(lo2, hi2, Math::ilerp<T, Q>(val, lo1, hi1));
}

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) noexcept {
	return (lo <= val) && (val <= hi);
}


template<typename T>
constexpr T abs(const T& val) noexcept {
	return val < T() ? -val : val;
}

inline float abs(float val) noexcept {
	return std::abs(val);
}

inline double abs(double val) noexcept {
	return std::abs(val);
}


template<typename T>
constexpr T inv(const T& val) noexcept {
	return T(1) / val;
}


template<typename T>
constexpr T mod(const T& num, const T& den) noexcept {
	return num % den;
}

inline float mod(float num, float den) noexcept {
	return std::fmod(num, den);
}

inline double mod(double num, double den) noexcept {
	return std::fmod(num, den);
}



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

inline float frac(float x) noexcept {
	return x - trunc(x);
}
inline double frac(double x) noexcept {
	return x - trunc(x);
}



template<typename T>
constexpr T gcd(const T& a, const T& b) noexcept {
	if(!static_cast<bool>(a)) return b; //Can't modulus by 0!
	if(!static_cast<bool>(b)) return a; //Can't modulus by 0!

	T x = Math::abs(a), y = Math::abs(b);
	T r = {}; //Reminder
	while(static_cast<bool>((r = Math::mod(x, y)))) {
		x = y;
		y = r;
	}

	return y;
}

template<typename T>
constexpr T lcm(const T& a, const T& b) noexcept {
	const T GCD = Math::gcd(a, b);
	return GCD != T() ? (a / GCD * b) : T();
}



template<typename T>
constexpr T sawtooth(const T& period, const T& value) noexcept {
	return Math::mod(Math::mod(value, period) + period, period);
}

template<typename T, typename R>
constexpr R sawtooth(const T& period, const R& amplitude, const T& value) noexcept {
	return sawtooth(period, value) * amplitude / period;
}

template<typename T>
constexpr T triangle(const T& period, const T& value) noexcept {
	auto x = 2 * sawtooth(period, value);

	if(Math::abs(x) > Math::abs(period)) {
		x = 2 * period - x;
	} 

	return x;
}

template<typename T, typename R>
constexpr R triangle(const T& period, const R& amplitude, const T& value) noexcept {
	return triangle(period, value) * amplitude / period;
}

}	