#include "Factor.h"

#include "Rational.h"
#include "Vector.h"
#include "Absolute.h"

#include <cmath>

namespace Zuazo::Math {

template<typename T>
constexpr T mod(const T& num, const T& den) {
	return num % den;
}

inline float mod(float num, float den) noexcept {
	return std::fmod(num, den);
}

inline double mod(double num, double den) noexcept {
	return std::fmod(num, den);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> mod(const Rational<num_t, den_t>& num, const Rational<num_t, den_t>& den) {
	return Rational<num_t, den_t>(
		(static_cast<intmax_t>(num.getNumerator()) * den.getDenominator()) % 
		(static_cast<intmax_t>(den.getNumerator()) * num.getDenominator()) ,
		static_cast<intmax_t>(num.getDenominator()) * den.getDenominator()
	);
}


template<typename T>
constexpr T gcd(const T& a, const T& b) {
	if(!static_cast<bool>(a)) return b; //Can't modulus by 0!
	if(!static_cast<bool>(b)) return a; //Can't modulus by 0!

	T x = abs(a), y = abs(b);
	T r = {}; //Reminder
	while(static_cast<bool>((r = mod(x, y)))) {
		x = y;
		y = r;
	}

	return y;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> gcd(const Rational<num_t, den_t>& a, const Rational<num_t, den_t>& b) {
	return Rational<num_t, den_t>(
		gcd(a.getNumerator(), b.getNumerator()),
		lcm(a.getNumerator(), b.getNumerator())
	);
}


template<typename T>
constexpr T lcm(const T& a, const T& b) {
	const T GCD = Math::gcd(a, b);
	return GCD != T() ? (a / GCD * b) : T();
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> lcm(const Rational<num_t, den_t>& a, const Rational<num_t, den_t>& b) {
	return Rational<num_t, den_t>(
		lcm(a.getNumerator(), b.getNumerator()),
		gcd(a.getNumerator(), b.getNumerator())
	);
}

}