#include "Funcs.h"

namespace Zuazo::Math {

/*
 * Constructors
 */

template<typename T>
constexpr Rational<T>::Rational() :
	m_num(0),
	m_den(1)
{
}

template<typename T>
constexpr Rational<T>::Rational(Integer num, Integer den) :
	m_num(num),
	m_den(den)
{
	//Take out the common factor
	Integer hcd = gcd(m_num, m_den);
	if(hcd > 1){
		m_num /= hcd;
		m_den /= hcd;
	}

	//Only numerator should be negative
	if(m_den < 0){
		m_num = -m_num;
		m_den = -m_den;
	}
}

template<typename T>
constexpr Rational<T>::Rational(Integer number) :
	m_num(number),
	m_den(1)
{
}

template<typename T>
constexpr Rational<T>::Rational(Real number) : Rational() {
	/*
	 * This code is based on:
	 * https://rosettacode.org/wiki/Convert_decimal_number_to_rational
	 */

	if(number == -std::numeric_limits<Real>::infinity()){ m_num = -1; m_den = 0; } //Minus infinity
	else if(number == +std::numeric_limits<Real>::infinity()){ m_num = +1; m_den = 0; } //Plus infinity
	else if(isnan(number)){ m_num = 0; m_den = 0; } //NAN
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
		m_den = k[1];
		m_num = neg ? -h[1] : h[1];
	}
}

template<typename T>
constexpr typename Rational<T>::Integer Rational<T>::getNumerator() const{
	return m_num;
}

template<typename T>
constexpr typename Rational<T>::Integer Rational<T>::getDenominator() const{
	return m_den;
}

template<typename T>
constexpr bool Rational<T>::isInteger() const{
	return m_den == 1;
}

/*
 * Casting operators
 */

template<typename T>
constexpr Rational<T>::operator bool() const{
	return m_num;
}

template<typename T>
constexpr Rational<T>::operator Integer() const{
	Integer result(0);

	if(m_den)           result = m_num / m_den;
	else if(m_num > 0)  result = std::numeric_limits<Integer>::max();
	else if(m_num < 0)  result = std::numeric_limits<Integer>::min();
	//else              result = 0;

	return result;
}

template<typename T>
constexpr Rational<T>::operator Real() const{
	return static_cast<Real>(m_num) / m_den; 
}

template<typename T>
template<typename Q>
constexpr Rational<T>::operator Rational<Q>() const{
	return Rational<Q>(
		static_cast<Q>(m_num),
		static_cast<Q>(m_den)
	);
}

/*
 * Arithmetic operators
 */

template<typename T>
constexpr Rational<T> operator+(const Rational<T>& left, const Rational<T>& right){
	return Rational<T>(
		left.m_num * right.m_den + right.m_num * left.m_den,
		left.m_den * right.m_den
	);
}

template<typename T>
constexpr Rational<T> operator-(const Rational<T>& left, const Rational<T>& right){
	return Rational<T>(
		left.m_num * right.m_den - right.m_num * left.m_den,
		left.m_den * right.m_den
	);
}

template<typename T>
constexpr Rational<T> operator*(const Rational<T>& left, const Rational<T>& right){
	return Rational<T>(
		left.m_num * right.m_num,
		left.m_den * right.m_den
	);
}

template<typename T>
constexpr Rational<T> operator/(const Rational<T>& left, const Rational<T>& right){
	return Rational<T>(
		left.m_num * right.m_den,
		right.m_num * left.m_den
	);
}

template<typename T>
constexpr Rational<T> Rational<T>::operator-(){
	return Rational<T>(-m_num, m_den);
}

template<typename T>
constexpr Rational<T>& Rational<T>::operator+=(const Rational<T>& right){
	(*this) = (*this) + right;
	return (*this);
}

template<typename T>
constexpr Rational<T>& Rational<T>::operator-=(const Rational<T>& right){
	(*this) = (*this) - right;
	return (*this);
}

template<typename T>
constexpr Rational<T>& Rational<T>::operator*=(const Rational<T>& right){
	(*this) = (*this) * right;
	return (*this);
}

template<typename T>
constexpr Rational<T>& Rational<T>::operator/=(const Rational<T>& right){
	(*this) = (*this) / right;
	return (*this);
}

/*
 * Comparison operators
 */

template<typename T>
constexpr int operator==(const Rational<T>& left, const Rational<T>& right){
	return left.m_num == right.m_num && left.m_den == right.m_den;
}

template<typename T>
constexpr int operator!=(const Rational<T>& left, const Rational<T>& right){
	return left.m_num != right.m_num || left.m_den != right.m_den;
}

template<typename T>
constexpr int operator<(const Rational<T>& left, const Rational<T>& right){
	return left.m_num * right.m_den < right.m_num * left.m_den;
}

template<typename T>
constexpr int operator<=(const Rational<T>& left, const Rational<T>& right){
	return left.m_num * right.m_den <= right.m_num * left.m_den;
}

template<typename T>
constexpr int operator>(const Rational<T>& left, const Rational<T>& right){
	return left.m_num * right.m_den > right.m_num * left.m_den;
}

template<typename T>
constexpr int operator>=(const Rational<T>& left, const Rational<T>& right){
	return left.m_num * right.m_den >= right.m_num * left.m_den;
}

}

namespace std {

template<typename T>
constexpr size_t hash<Zuazo::Math::Rational<T>>::operator()(const Zuazo::Math::Rational<T>& rat) const {
	if constexpr (sizeof(size_t) > ( sizeof(T) / 2 )){
		//It does not have trivial hash
		return std::hash<T>(rat.m_num) ^ std::hash<T>(rat.m_den);
	} else {
		//It has a trivial hash
		constexpr int shift = sizeof(size_t) * 8 / 2; //Bits to shift (half the bits of size_t)
		return (static_cast<size_t>(rat.m_num) << shift) | static_cast<size_t>(rat.m_den);
	}
}

}