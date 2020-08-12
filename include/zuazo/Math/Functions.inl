#include "Functions.h"

#include <functional>
#include <cmath>

namespace Zuazo::Math {

template<typename T>
constexpr const T& min(const T& a, const T& b) {
	return std::min(a, b);
}

template<typename T>
constexpr const T& min(std::initializer_list<T> ilist) {
	return std::min(ilist);
}


template<typename T>
constexpr const T& max(const T& a, const T& b) {
	return std::max(a, b);
}

template<typename T>
constexpr const T& max(std::initializer_list<T> ilist) {
	return std::max(ilist);
}



template<typename T>
constexpr const T& clamp(const T& val, const T& lo, const T& hi) {
	return min(hi, max(lo, val));
}

template<typename T, typename Q>
constexpr T lerp(const T& a, const T& b, const Q& r) {
	return static_cast<T>(a + (b - a) * r);
}

template<typename T, typename Q>
constexpr Q ilerp(const T& val, const T& a, const T& b) {
	return static_cast<Q>(val - a) / static_cast<Q>(b - a);
}

template<typename T, typename Q>
constexpr T map(const T& val, const T& lo1, const T& hi1, const T& lo2, const T& hi2) {
	return lerp(lo2, hi2, ilerp<T, Q>(val, lo1, hi1));
}

template<typename T>
constexpr bool isInRange(const T& val, const T& lo, const T& hi) {
	return (lo <= val) && (val <= hi);
}


template<typename T>
constexpr T abs(const T& val) {
	return val < T() ? -val : val;
}

inline float abs(float val) {
	return std::abs(val);
}

inline double abs(double val) {
	return std::abs(val);
}


template<typename T>
constexpr T inv(const T& val) {
	return T(1) / val;
}


template<typename T>
constexpr T mod(const T& num, const T& den) {
	return num % den;
}

inline float mod(float num, float den) {
	return std::fmod(num, den);
}

inline double mod(double num, double den) {
	return std::fmod(num, den);
}



inline float floor(float x) {
	return std::floor(x);
}

inline double floor(double x) {
	return std::floor(x);
}

inline float trunc(float x) {
	return std::trunc(x);
}

inline double trunc(double x) {
	return std::trunc(x);
}

inline float ceil(float x) {
	return std::ceil(x);
}

inline double ceil(double x) {
	return std::ceil(x);
}

inline float round(float x) {
	return std::round(x);
}

inline double round(double x) {
	return std::round(x);
}

inline float frac(float x) {
	return x - trunc(x);
}
inline double frac(double x) {
	return x - trunc(x);
}



template<typename T>
constexpr T gcd(const T& a, const T& b) {
	if(!static_cast<bool>(a)) return b; //Can't modulus by 0!
	if(!static_cast<bool>(b)) return a; //Can't modulus by 0!

	T x = Math::abs(a), y = Math::abs(b);
	T r = {}; //Reminder
	while(static_cast<bool>(r = mod(x, y))) {
		x = y;
		y = r;
	}

	return y;
}

template<typename T>
constexpr T lcm(const T& a, const T& b) {
	const T GCD = Math::gcd(a, b);
	return GCD != T() ? (a / GCD * b) : T();
}

}	