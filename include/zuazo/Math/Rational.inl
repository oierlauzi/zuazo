#include "Rational.h"

#include "Functions.h"

namespace Zuazo::Math {

/*
 * Constructors
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational() noexcept
	: m_num(0)
	, m_den(1)
{
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational(Num num, Den den) noexcept
	: m_num(num)
	, m_den(den)
{
	//Take out the common factor
	Integer gcd = Math::gcd(m_num, m_den);
	if(gcd > 1){
		m_num /= gcd;
		m_den /= gcd;
	}

	//Only numerator should be negative
	if(m_den < Den(0)) {
		m_num = -m_num;
		m_den = -m_den;
	}
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational(Num number) noexcept
	: m_num(number)
	, m_den(1)
{
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational(Real number) noexcept
	: Rational() 
{
	/*
	 * This code is based on:
	 * https://rosettacode.org/wiki/Convert_decimal_number_to_rational
	 */

	if(number == -std::numeric_limits<Real>::infinity()){ m_num = Num(-1); m_den = Den(0); } //Minus infinity
	else if(number == +std::numeric_limits<Real>::infinity()){ m_num = Num(+1); m_den = Den(0); } //Plus infinity
	else if(std::isnan(number)){ m_num = Num(0); m_den = Den(0); } //NAN
	else{
		const size_t precision=64;
		const int32_t MAX_DENOMINATOR=4096;

		bool neg = number < 0;
		if (neg) {
			number = -number; //Set it to the absolute value
		}

		//Multiply numerator and denominator by 2 until numerator has no decimals left
		Integer n=1;
		while (number != floor(number)) {
			n <<= 1; // n *= 2;
			number *= 2;
		}
		Integer d = static_cast<Integer>(number);

		Integer h[3] = { 0, 1, 0 };
		Integer k[3] = { 1, 0, 0 };
		for (u_int32_t i = 0; i < precision; i++) {
			Integer a = n ? d / n : 0;
			if (i && !a)
				break;

			Integer x = d;
			d = n;
			n = x % n;
			x = a;

			if (k[1] * a + k[0] >= MAX_DENOMINATOR) {
				x = (MAX_DENOMINATOR - k[0]) / k[1];
				if (x * 2 >= a || k[1] >= MAX_DENOMINATOR)
					i = precision+1;
				else
					break;
			}

			h[2] = x * h[1] + h[0]; h[0] = h[1]; h[1] = h[2];
			k[2] = x * k[1] + k[0]; k[0] = k[1]; k[1] = k[2];
		}

		//Save the values
		m_num = Num(neg ? -h[1] : h[1]);
		m_den = Den(k[1]);
	}
}

template<typename num_t, typename den_t>
template<intmax_t num, intmax_t den>
constexpr Rational<num_t, den_t>::Rational(std::ratio<num, den>)  noexcept
	: Rational(Num(num), Den(den))
{
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr Rational<num_t, den_t>::Rational(const Rational<num_t2, den_t2>& rat) noexcept
	: m_num(static_cast<num_t>(rat.getNumerator()))
	, m_den(static_cast<den_t>(rat.getDenominator()))
{
}

template<typename num_t, typename den_t>
constexpr typename Rational<num_t, den_t>::Num Rational<num_t, den_t>::getNumerator() const noexcept {
	return m_num;
}

template<typename num_t, typename den_t>
constexpr typename Rational<num_t, den_t>::Den Rational<num_t, den_t>::getDenominator() const noexcept {
	return m_den;
}

template<typename num_t, typename den_t>
constexpr bool Rational<num_t, den_t>::isInteger() const noexcept {
	return m_den == 1;
}


template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::abs() const noexcept {
	return Rational<num_t, den_t>(Math::abs(m_num), m_den);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::inv() const noexcept {
	return Rational<num_t, den_t>(m_den, m_num);
}


template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::floor() const noexcept {
	return *this < 0 ? -trunc(-(*this) + Rational(m_den - 1, m_den)) : trunc(*this);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::trunc() const noexcept {
	return Rational<num_t, den_t>(
		static_cast<Num>(m_num / m_den),
		Den(1)
	);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::ceil() const noexcept {
	return *this < 0 ? -trunc(-(*this)) : trunc(*this + Rational(m_den - 1, m_den));
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::round() const noexcept {
	return (*this + Rational(1, 2)).floor();
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::frac() const noexcept {
	return Rational<num_t, den_t>(
		Math::mod(m_num, m_den),
		m_den
	);
}

/*
 * Casting operators
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator bool() const noexcept {
	return m_num != Num(0);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator Integer() const noexcept {
	Integer result(0);

	if(m_den)           result = m_num / m_den;
	else if(m_num > 0)  result = std::numeric_limits<Integer>::max();
	else if(m_num < 0)  result = std::numeric_limits<Integer>::min();
	//else              result = 0;

	return result;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator Real() const noexcept {
	return static_cast<Real>(m_num) / m_den; 
}

/*
 * Arithmetic operators
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::operator-() noexcept {
	return Rational(-m_num, m_den);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& Rational<num_t, den_t>::operator+=(const Rational& right) noexcept {
	(*this) = (*this) + right;
	return (*this);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& Rational<num_t, den_t>::operator-=(const Rational& right) noexcept {
	(*this) = (*this) - right;
	return (*this);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& Rational<num_t, den_t>::operator*=(const Rational& right) noexcept {
	(*this) = (*this) * right;
	return (*this);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& Rational<num_t, den_t>::operator/=(const Rational& right) noexcept {
	(*this) = (*this) / right;
	return (*this);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& Rational<num_t, den_t>::operator%=(const Rational& right) noexcept {
	(*this) = (*this) % right;
	return (*this);
}


template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr typename Rational<num_t, den_t>::template CommonType<Rational<num_t2, den_t2>>
Rational<num_t, den_t>::operator+(const Rational<num_t2, den_t2>& right) const noexcept {
	return CommonType<Rational<num_t2, den_t2>> (
		m_num * right.m_den + right.m_num * m_den,
		m_den * right.m_den
	);
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr typename Rational<num_t, den_t>::template CommonType<Rational<num_t2, den_t2>> 
Rational<num_t, den_t>::operator-(const Rational<num_t2, den_t2>& right) const noexcept {
	return CommonType<Rational<num_t2, den_t2>>(
		m_num * right.m_den - right.m_num * m_den,
		m_den * right.m_den
	);
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr typename Rational<num_t, den_t>::template CommonType<Rational<num_t2, den_t2>> 
Rational<num_t, den_t>::operator*(const Rational<num_t2, den_t2>& right) const noexcept {
	return CommonType<Rational<num_t2, den_t2>>(
		m_num * right.m_num,
		m_den * right.m_den
	);
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr typename Rational<num_t, den_t>::template CommonType<Rational<num_t2, den_t2>> 
Rational<num_t, den_t>::operator/(const Rational<num_t2, den_t2>& right) const noexcept {
	return CommonType<Rational<num_t2, den_t2>>(
		m_num * right.m_den,
		right.m_num * m_den
	);
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr typename Rational<num_t, den_t>::template CommonType<Rational<num_t2, den_t2>> 
Rational<num_t, den_t>::operator%(const Rational<num_t2, den_t2>& right) const noexcept {
	return CommonType<Rational<num_t2, den_t2>>(
		(m_num * right.m_den) % (right.m_num * m_den),
		right.m_den * m_den
	);
}

/*
 * Comparison operators
 */

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator==(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num == right.m_num && m_den == right.m_den;
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator!=(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num != right.m_num || m_den != right.m_den;
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator<(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num * right.m_den < right.m_num * m_den;
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator<=(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num * right.m_den <= right.m_num * m_den;
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator>(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num * right.m_den > right.m_num * m_den;
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr int Rational<num_t, den_t>::operator>=(const Rational<num_t2, den_t2>& right) const noexcept {
	return m_num * right.m_den >= right.m_num * m_den;
}



template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::min() {
	return Rational<num_t, den_t>(-1, 0);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::max() {
	return Rational<num_t, den_t>(1, 0);
}



template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> mod(const Rational<num_t, den_t>& num, const Rational<num_t, den_t>& den) noexcept {
	return num % den;
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> abs(const Rational<num_t, den_t>& r) noexcept {
	return r.abs();
}


template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> inv(const Rational<num_t, den_t>& r) noexcept {
	return r.inv();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> floor(const Rational<num_t, den_t>& x) noexcept {
	return x.floor();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> trunc(const Rational<num_t, den_t>& x) noexcept {
	return x.trunc();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> ceil(const Rational<num_t, den_t>&x) noexcept {
	return x.ceil();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> round(const Rational<num_t, den_t>& x) noexcept {
	return x.round();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> frac(const Rational<num_t, den_t>& x) noexcept {
	return x.frac();
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> gcd(const Rational<den_t, num_t>& a, const Rational<den_t, num_t>& b) noexcept {
	return Rational<den_t, num_t>(
		Math::gcd(a.getNumerator(), b.getNumerator()),
		Math::lcm(a.getDenominator(), b.getDenominator())
	);
}

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> lcm(const Rational<den_t, num_t>& a, const Rational<den_t, num_t>& b) noexcept {
	return Rational<den_t, num_t>(
		Math::lcm(a.getNumerator(), b.getNumerator()),
		Math::gcd(a.getDenominator(), b.getDenominator())
	);
}



template<typename num_t, typename den_t>
inline std::ostream& operator<<(std::ostream& os, const Rational<num_t, den_t>& rat) {
	os << rat.getNumerator() << "/" << rat.getDenominator();

	return os;
}

}
