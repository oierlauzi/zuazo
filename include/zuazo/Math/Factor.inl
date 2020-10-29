#include "Factor.h"

#include <cmath>

namespace Zuazo::Math {

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

}