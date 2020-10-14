#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <numeric>
#include <type_traits>
#include <ratio>
#include <ostream>

namespace Zuazo::Math {

/**
 * \brief 
 * Represents a fractional number
 * 
 * \details
 * A fractional number represented by two integers of type T. It is always represented 
 * in the most reduced form. Sign will always be assigned to the numerator.
 * 
 * \arg num_t
 * Type of the numerator to be used
 *
 * \arg den_t
 * Type of the denominator to be used
 */

template<typename num_t = int_fast32_t, typename den_t = num_t>
class Rational{
public:
	static_assert(std::is_integral<num_t>::value, "num_t must be an integer");
	static_assert(std::is_integral<den_t>::value, "den_t must be an integer");

	//////////////////////////////////////////////
	//				TYPE DEFINITIONS			//
	//////////////////////////////////////////////

	/**
	 * \brief
	 * Type used to represent the numerator.
	 */
	using Num = num_t;

		/**
	 * \brief
	 * Type used to represent the denominator.
	 */
	using Den = den_t;

	/**
	 * \brief
	 * Type used to represent integers.
	 */
	using Integer = intmax_t;

	/**
	 * \brief
	 * Type used to represent real numbers.
	 */
	using Real = double;

	template<typename Rational2>
	using CommonType = Rational<typename std::common_type<Num, typename Rational2::Num>::type,
								typename std::common_type<Den, typename Rational2::Den>::type >;

	//////////////////////////////////////////////
	//				CONSTRUCTORS				//
	//////////////////////////////////////////////

	/**
	 * \brief
	 * Default constructor. Assigns the number 0.
	 */
	constexpr Rational() noexcept;

	/**
	 * \brief
	 * Constructs a rational number given its numerator and denominator.
	 * 
	 * \note
	 * Fraction will be reduced and sign will be assigned to the numerator so that
	 * Rational(6, -12) is equivalent to Rational(-1, 2)
	 * 
	 * \param[in] num
	 * The numerator part of the fraction
	 * 
	 * \param[in] den
	 * The denominator part of the fraction
	 */
	constexpr Rational(Num num, Den den) noexcept;

	/**
	 * \brief
	 * Constructs a rational number given an integer
	 * 
	 * \param[in] number
	 * The integer which will get assigned to the rational number
	 */
	constexpr Rational(Num number) noexcept;

	/**
	 * \brief
	 * Constructs a rational number given an real number
	 * 
	 * \details
	 * Numerator and denominator are aproximated using the following algorithm:
	 * https://rosettacode.org/wiki/Convert_decimal_number_to_rational
	 * 
	 * \param[in] number
	 * The real number which will get assigned to the rational number
	 */
	explicit constexpr Rational(Real number) noexcept;

	/**
	 * \brief
	 * Constructs a rational from a compile time std::ratio
	 * 
	 * \param[in] rat
	 * The compile time std::ratio from which it is constructed
	 */

	template<intmax_t num, intmax_t den>
	constexpr Rational(std::ratio<num, den>) noexcept;

	/**
	 * \brief
	 * Copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	constexpr Rational(const Rational& rat) noexcept = default;

	/**
	 * \brief
	 * Cross-type copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	template<typename num_t2, typename den_t2>
	constexpr Rational(const Rational<num_t2, den_t2>& rat) noexcept;

	/**
	 * \brief
	 * Destructor
	 */
	~Rational() = default;

	/**
	 * \brief
	 * Copy assigment operator
	 * \param[in] other
	 * The rational to be copied
	 */
	constexpr Rational& operator=(const Rational& other) noexcept = default;

	//////////////////////////////////////////////
	//				PUBLIC METHODS				//
	//////////////////////////////////////////////

	/**
	 * \brief
	 * Getter for the numerator part of the rational number
	 * 
	 * \returns
	 * The numerator part of the rational number
	 */
	constexpr Num getNumerator() const noexcept;

	/**
	 * \brief
	 * Getter for the denominator part of the rational number
	 * 
	 * \returns
	 * The denominator part of the rational number
	 */
	constexpr Den getDenominator() const noexcept;

	/**
	 * \brief
	 * Returns wether or not it is an integer number representation
	 */
	constexpr bool isInteger() const noexcept;


	/**
	 * \brief
	 * Returns the absolute (unsigned) value
	 */
	constexpr Rational abs() const noexcept;

	/**
	 * \brief
	 * Returns the inverse value (Numerator <-> Denominator)
	 */
	constexpr Rational inv() const noexcept;

	/**
	 * \brief
	 * Returns the closest integer value not greater than itself
	 * 
	 * \see
	 * https://en.cppreference.com/w/cpp/numeric/math/floor 
	 */
	constexpr Rational floor() const noexcept;

	/**
	 * \brief
	 * Removes the fractional part so that the result is the closest
	 * integer towards zero
	 * 
	 * \see
	 * https://en.cppreference.com/w/cpp/numeric/math/trunc 
	 */
	constexpr Rational trunc() const noexcept;

	/**
	 * \brief
	 * Returns the closest integer value not less than itself
	 * 
	 * \see
	 * https://en.cppreference.com/w/cpp/numeric/math/ceil 
	 */
	constexpr Rational ceil() const noexcept;

	/**
	 * \brief
	 * Returns the closest integer value
	 * 
	 * \see
	 * https://en.cppreference.com/w/cpp/numeric/math/round
	 */
	constexpr Rational round() const noexcept;

	/**
	 * \brief
	 * Returns the fractional part
	 */
	constexpr Rational frac() const noexcept;

	//////////////////////////////////////////////
	//				CAST OPERATORS				//
	//////////////////////////////////////////////

	constexpr operator bool() const noexcept;
	explicit constexpr operator Integer() const noexcept;
	explicit constexpr operator Real() const noexcept;

	//////////////////////////////////////////////
	//			ARITHMETIC OPERATORS			//
	//////////////////////////////////////////////

	constexpr Rational operator-() noexcept;
	constexpr Rational& operator+=(const Rational& right) noexcept;
	constexpr Rational& operator-=(const Rational& right) noexcept;
	constexpr Rational& operator*=(const Rational& right) noexcept;
	constexpr Rational& operator/=(const Rational& right) noexcept;
	constexpr Rational& operator%=(const Rational& right) noexcept;

	template<typename num_t2, typename den_t2>
	constexpr CommonType<Rational<num_t2, den_t2>> operator+(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<Rational<num_t2, den_t2>> operator-(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<Rational<num_t2, den_t2>> operator*(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<Rational<num_t2, den_t2>> operator/(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<Rational<num_t2, den_t2>> operator%(const Rational<num_t2, den_t2>& right) const noexcept;

	//////////////////////////////////////////////
	//				COMPARE OPERATORS			//
	//////////////////////////////////////////////

	template<typename num_t2, typename den_t2>
	constexpr int operator==(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr int operator!=(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr int operator<(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr int operator<=(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr int operator>(const Rational<num_t2, den_t2>& right) const noexcept;
	template<typename num_t2, typename den_t2>
	constexpr int operator>=(const Rational<num_t2, den_t2>& right) const noexcept;
	
private:
	Num m_num;
	Den m_den;
};

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> mod(const Rational<num_t, den_t>& num, const Rational<num_t, den_t>& den) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> abs(const Rational<num_t, den_t>& r) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> inv(const Rational<num_t, den_t>& r) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> floor(const Rational<num_t, den_t>& x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> trunc(const Rational<num_t, den_t>& x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> ceil(const Rational<num_t, den_t>&x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> round(const Rational<num_t, den_t>& x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> frac(const Rational<num_t, den_t>& x) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> gcd(const Rational<den_t, num_t>& a, const Rational<den_t, num_t>& b) noexcept;

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> lcm(const Rational<den_t, num_t>& a, const Rational<den_t, num_t>& b) noexcept;



template<typename num_t, typename den_t>
std::ostream& operator<<(std::ostream& os, const Rational<num_t, den_t>& rat);

}

namespace Zuazo {

namespace Utils {

template<typename T>
class Any;

template<typename num_t, typename den_t>
constexpr Math::Rational<num_t, den_t> lowest(const Any<Math::Rational<num_t, den_t>>& any) noexcept;
template<typename num_t, typename den_t>
constexpr Math::Rational<num_t, den_t> highest(const Any<Math::Rational<num_t, den_t>>& any) noexcept;

}

}

#include "Rational.inl"
