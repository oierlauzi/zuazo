#pragma once

#include <math.h>
#include <stddef.h>
#include <sys/types.h>
#include <limits>
#include <numeric>
#include <type_traits>

namespace Zuazo::Math {

/**
 * \brief 
 * Represents a fractional number
 * 
 * \details
 * A fractional number represented by two integers of type T. It is always represented 
 * in the most reduced form. Sign will always be assigned to the numerator.
 * 
 * \arg T
 * Type of integer to be used. 
 * It must be a signed integer
 */

template<typename T>
class Rational{
public:
	static_assert(std::is_integral<T>::value, "Template parameter must be an integer type");
	static_assert(std::is_signed<T>::value, "Template parameter must a signed type");

	//////////////////////////////////////////////
	//				TYPE DEFINITIONS			//
	//////////////////////////////////////////////

	/**
	 * \brief
	 * Type used to represent integers.
	 */
	using Integer = T;

	/**
	 * \brief
	 * Type used to represent real numbers.
	 */
	using Real = double;

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
	constexpr Rational(Integer num, Integer den);

	/**
	 * \brief
	 * Constructs a rational number given an integer
	 * 
	 * \param[in] number
	 * The integer which will get assigned to the rational number
	 */
	constexpr Rational(Integer number);

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
	constexpr Rational(Real number);

	/**
	 * \brief
	 * Copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	Rational(const Rational<T>& rat)=default;

	/**
	 * \brief
	 * Cross-type copy constructor
	 * 
	 * \param[in] rat
	 * The rational to be copied
	 */
	template<typename Q>
	Rational(const Rational<Q>& rat);

	/**
	 * \brief
	 * Destructor
	 */
	~Rational()=default;

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
	constexpr Integer getNumerator() const;

	/**
	 * \brief
	 * Getter for the denominator part of the rational number
	 * 
	 * \returns
	 * The denominator part of the rational number
	 */
	constexpr Integer getDenominator() const;

	/**
	 * \brief
	 * Returns wether or not it is an integer number representation
	 */
	constexpr bool isInteger() const;

	//////////////////////////////////////////////
	//				CAST OPERATORS				//
	//////////////////////////////////////////////

	constexpr operator bool() const;
	explicit constexpr operator Integer() const;
	explicit constexpr operator Real() const;

	//////////////////////////////////////////////
	//			ARTHMETIC OPERATORS				//
	//////////////////////////////////////////////

	template<typename Q>
	friend constexpr Rational<Q> operator+(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr Rational<Q> operator-(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr Rational<Q> operator*(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr Rational<Q> operator/(const Rational<Q>& left, const Rational<Q>& right);

	constexpr Rational<T> operator-();
	constexpr Rational<T>& operator=(const Rational<T>& right) = default;
	constexpr Rational<T>& operator+=(const Rational<T>& right);
	constexpr Rational<T>& operator-=(const Rational<T>& right);
	constexpr Rational<T>& operator*=(const Rational<T>& right);
	constexpr Rational<T>& operator/=(const Rational<T>& right);

	template<typename Q>
	friend constexpr int operator==(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr int operator!=(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr int operator<(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr int operator<=(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr int operator>(const Rational<Q>& left, const Rational<Q>& right);
	template<typename Q>
	friend constexpr int operator>=(const Rational<Q>& left, const Rational<Q>& right);

	template<typename Q>
	friend std::hash<Rational<Q>>;
private:
	Integer m_num;
	Integer m_den;
};

typedef Rational<int64_t>	Rational128_t;
typedef Rational<int32_t>	Rational64_t;
typedef Rational<int16_t>	Rational32_t;
typedef Rational<int8_t>	Rational16_t;
typedef Rational<int_fast32_t> Rational_t;

}

#include "Rational.inl"
