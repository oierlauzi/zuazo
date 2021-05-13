#include "Rational.h"

#include "Rounding.h"
#include "Factor.h"

#include <charconv>

namespace Zuazo::Math {

/*
 * Constructors
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational()
	: m_num(0)
	, m_den(1)
{
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational(Num num, Den den)
	: m_num(num)
	, m_den(den)
{
	//Take out the common factor
	const auto gcd = Math::gcd(m_num, m_den);
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
constexpr Rational<num_t, den_t>::Rational(Num number)
	: m_num(number)
	, m_den(1)
{
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::Rational(Real number)
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
		constexpr size_t precision = 64;
		constexpr int32_t MAX_DENOMINATOR = 4096;

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
constexpr Rational<num_t, den_t>::Rational(std::ratio<num, den>) 
	: m_num(std::ratio<num, den>::num) //Already reduced
	, m_den(std::ratio<num, den>::den)
{
}

template<typename num_t, typename den_t>
template<typename num_t2, typename den_t2>
constexpr Rational<num_t, den_t>::Rational(const Rational<num_t2, den_t2>& rat)
	: m_num(static_cast<num_t>(rat.getNumerator())) //Already reduced
	, m_den(static_cast<den_t>(rat.getDenominator()))
{
}

/*
 * Getters
 */

template<typename num_t, typename den_t>
constexpr const typename Rational<num_t, den_t>::Num& Rational<num_t, den_t>::getNumerator() const noexcept {
	return m_num;
}

template<typename num_t, typename den_t>
constexpr const typename Rational<num_t, den_t>::Den& Rational<num_t, den_t>::getDenominator() const noexcept {
	return m_den;
}


/*
 * Casting operators
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator bool() const {
	return m_num != static_cast<Num>(0);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator Integer() const {
	Integer result(0);

	if(m_den)           result = m_num / m_den;
	else if(m_num > 0)  result = std::numeric_limits<Integer>::max();
	else if(m_num < 0)  result = std::numeric_limits<Integer>::min();
	//else              result = 0;

	return result;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>::operator Real() const {
	return static_cast<Real>(m_num) / static_cast<Real>(m_den); 
}


/*
 * Static constants
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::min() {
	return Rational<num_t, den_t>(-1, 0);
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> Rational<num_t, den_t>::max() {
	return Rational<num_t, den_t>(1, 0);
}


/*
 * Comparison operators
 */

template<typename num_t, typename den_t>
constexpr bool operator==(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return lhs.getNumerator() == rhs.getNumerator() && lhs.getDenominator() == rhs.getDenominator();
}

template<typename num_t, typename den_t>
constexpr bool operator!=(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return !(lhs == rhs);
}

template<typename num_t, typename den_t>
constexpr bool operator<(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return 	static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() 
		< 	static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator();
}

template<typename num_t, typename den_t>
constexpr bool operator<=(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return 	static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() 
		<= 	static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator();
}

template<typename num_t, typename den_t>
constexpr bool operator>(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return 	static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() 
		> 	static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator();
}

template<typename num_t, typename den_t>
constexpr bool operator>=(const Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return 	static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() 
		<= 	static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator();
}


/*
 * Arithmetic operators
 */

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> operator+(const Rational<num_t, den_t>& a) {
	return a;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t> operator-(const Rational<num_t, den_t>& a) {
	return Rational<num_t, den_t>(-a.getNumerator(), a.getDenominator());
}


template<typename num_t1, typename den_t1, typename num_t2, typename den_t2>
constexpr Rational<typename std::common_type<num_t1, num_t2>::type, typename std::common_type<den_t1, den_t2>::type>
operator+(const Rational<num_t1, den_t1>& lhs, const Rational<num_t2, den_t2>& rhs) {
	return Rational<intmax_t, intmax_t>(
		static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() +
		static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator(),
		static_cast<intmax_t>(lhs.getDenominator())*rhs.getDenominator()
	);
}

template<typename num_t1, typename den_t1, typename num_t2, typename den_t2>
constexpr Rational<typename std::common_type<num_t1, num_t2>::type, typename std::common_type<den_t1, den_t2>::type>
operator-(const Rational<num_t1, den_t1>& lhs, const Rational<num_t2, den_t2>& rhs) {
	return Rational<intmax_t, intmax_t>(
		static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator() - 
		static_cast<intmax_t>(rhs.getNumerator())*lhs.getDenominator(),
		static_cast<intmax_t>(lhs.getDenominator())*rhs.getDenominator()
	);
}

template<typename num_t1, typename den_t1, typename num_t2, typename den_t2>
constexpr Rational<typename std::common_type<num_t1, num_t2>::type, typename std::common_type<den_t1, den_t2>::type>
operator*(const Rational<num_t1, den_t1>& lhs, const Rational<num_t2, den_t2>& rhs) {
	return Rational<intmax_t, intmax_t>(
		static_cast<intmax_t>(lhs.getNumerator())*rhs.getNumerator(),
		static_cast<intmax_t>(lhs.getDenominator())*rhs.getDenominator()
	);
}

template<typename num_t1, typename den_t1, typename num_t2, typename den_t2>
constexpr Rational<typename std::common_type<num_t1, num_t2>::type, typename std::common_type<den_t1, den_t2>::type>
operator/(const Rational<num_t1, den_t1>& lhs, const Rational<num_t2, den_t2>& rhs) {
	return Rational<intmax_t, intmax_t>(
		static_cast<intmax_t>(lhs.getNumerator())*rhs.getDenominator(),
		static_cast<intmax_t>(lhs.getDenominator())*rhs.getNumerator()
	);
}


template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& operator+=(Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return lhs = lhs + rhs;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& operator-=(Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return lhs = lhs - rhs;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& operator*=(Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return lhs = lhs * rhs;
}

template<typename num_t, typename den_t>
constexpr Rational<num_t, den_t>& operator/=(Rational<num_t, den_t>& lhs, const Rational<num_t, den_t>& rhs) {
	return lhs = lhs / rhs;
}


template<typename num_t, typename den_t>
inline std::ostream& operator<<(std::ostream& os, const Rational<num_t, den_t>& rat) {
	os << rat.getNumerator() << "/" << rat.getDenominator();

	return os;
}

template<typename num_t, typename den_t>
inline bool fromString(std::string_view str, Rational<num_t, den_t>& res) noexcept {
	num_t num;
	den_t den;
	std::from_chars_result ret;
	ret.ptr = str.cbegin();
	
	//Parse the numerator
	ret = std::from_chars(ret.ptr, str.cend(), num);
	if(ret.ec != std::errc()) {
		return false;
	}

	//Expect a slash afterwards
	if(ret.ptr != str.cend()) {
		if(*(ret.ptr++) != '/') {
			return false;
		}
	}

	//Parse the denominator
	ret = std::from_chars(ret.ptr, str.cend(), den);
	if(ret.ec != std::errc()) {
		return false;
	}

	res = Rational<num_t, den_t>(num, den);
	return true;
}

}
