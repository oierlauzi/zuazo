#pragma once

#include <math.h>
#include <stddef.h>
#include <sys/types.h>
#include <limits>
#include <numeric>

namespace Zuazo::Utils{

class Rational{
public:
	typedef int64_t Integer;
	typedef double Real;

	constexpr Rational();
	constexpr Rational(Integer num, Integer den);
	constexpr Rational(Integer number);
	constexpr Rational(Real number);
	Rational(const Rational& rat)=default;
	~Rational()=default;

	constexpr Integer getNumerator() const;
	constexpr Integer getDenominator() const;

	constexpr operator bool() const;
	constexpr operator Integer() const;
	constexpr operator Real() const;

	friend constexpr Rational operator+(const Rational& left, const Rational& right);
	friend constexpr Rational operator-(const Rational& left, const Rational& right);
	friend constexpr Rational operator*(const Rational& left, const Rational& right);
	friend constexpr Rational operator/(const Rational& left, const Rational& right);
	constexpr Rational operator-();
	constexpr Rational& operator+=(const Rational& right);
	constexpr Rational& operator-=(const Rational& right);
	constexpr Rational& operator*=(const Rational& right);
	constexpr Rational& operator/=(const Rational& right);

	friend constexpr int operator==(const Rational& left, const Rational& right);
	friend constexpr int operator!=(const Rational& left, const Rational& right);
	friend constexpr int operator<(const Rational& left, const Rational& right);
	friend constexpr int operator<=(const Rational& left, const Rational& right);
	friend constexpr int operator>(const Rational& left, const Rational& right);
	friend constexpr int operator>=(const Rational& left, const Rational& right);

private:
	Integer m_num;
	Integer m_den;

    constexpr void simplify();
};

}

#include "Rational.inl"
