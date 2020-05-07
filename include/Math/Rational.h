#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <numeric>
#include <type_traits>
#include <ratio>
#include <string>

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

	template<typename num_t2, typename den_t2>
	using CommonType = Rational<typename std::common_type<Num, num_t2>::type,
								typename std::common_type<Den, den_t2>::type>;

	//////////////////////////////////////////////
	//				CONSTRUCTORS				//
	//////////////////////////////////////////////

	/**
	 * \brief
	 * Default constructor. Assigns the number 0.
	 */
	constexpr Rational();

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
	constexpr Rational(Num num, Den den);

	/**
	 * \brief
	 * Constructs a rational number given an integer
	 * 
	 * \param[in] number
	 * The integer which will get assigned to the rational number
	 */
	constexpr Rational(Num number);

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
	explicit constexpr Rational(Real number);

	/**
	 * \brief
	 * Constructs a rational from a compile time std::ratio
	 * 
	 * \param[in] rat
	 * The compile time std::ratio from which it is constructed
	 */

	template<intmax_t num, intmax_t den>
	constexpr Rational(std::ratio<num, den>);

	/**
	 * \brief
	 * Copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	constexpr Rational(const Rational& rat)=default;

	/**
	 * \brief
	 * Cross-type copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	template<typename num_t2, typename den_t2>
	constexpr Rational(const Rational<num_t2, den_t2>& rat);

	/**
	 * \brief
	 * Destructor
	 */
	~Rational()=default;

	/**
	 * \brief
	 * Copy assigment operator
	 * \param[in] other
	 * The rational to be copied
	 */
	constexpr Rational& operator=(const Rational& other) = default;

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
	constexpr Num getNumerator() const;

	/**
	 * \brief
	 * Getter for the denominator part of the rational number
	 * 
	 * \returns
	 * The denominator part of the rational number
	 */
	constexpr Den getDenominator() const;

	/**
	 * \brief
	 * Returns wether or not it is an integer number representation
	 */
	constexpr bool isInteger() const;

	//////////////////////////////////////////////
	//				CAST OPERATORS				//
	//////////////////////////////////////////////

	constexpr operator bool() const;
	explicit constexpr operator Num() const;
	explicit constexpr operator Real() const;

	//////////////////////////////////////////////
	//			ARITHMETIC OPERATORS			//
	//////////////////////////////////////////////

	constexpr Rational operator-();
	constexpr Rational& operator+=(const Rational& right);
	constexpr Rational& operator-=(const Rational& right);
	constexpr Rational& operator*=(const Rational& right);
	constexpr Rational& operator/=(const Rational& right);

	template<typename num_t2, typename den_t2>
	constexpr CommonType<num_t2, den_t2> operator+(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<num_t2, den_t2> operator-(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<num_t2, den_t2> operator*(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr CommonType<num_t2, den_t2> operator/(const Rational<num_t2, den_t2>& right) const;

	//////////////////////////////////////////////
	//				COMPARE OPERATORS			//
	//////////////////////////////////////////////

	template<typename num_t2, typename den_t2>
	constexpr int operator==(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr int operator!=(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr int operator<(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr int operator<=(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr int operator>(const Rational<num_t2, den_t2>& right) const;
	template<typename num_t2, typename den_t2>
	constexpr int operator>=(const Rational<num_t2, den_t2>& right) const;
	
private:
	Num m_num;
	Den m_den;
};

template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> inv(const Rational<num_t, den_t>& r);

}

namespace Zuazo::Utils {

template<typename T>
std::string	toString(const Math::Rational<T>& rat);

}


#include "Rational.inl"
