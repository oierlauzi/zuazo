#pragma once

#include <math.h>
#include <stddef.h>
#include <sys/types.h>
#include <limits>
#include <numeric>

namespace Zuazo::Math {

template<typename T>
class Rational{
public:
	using Integer = T;
	using Real = double;

	constexpr Rational();
	constexpr Rational(Integer num, Integer den);
	constexpr Rational(Integer number);
	constexpr Rational(Real number);
	Rational(const Rational<T>& rat)=default;
	~Rational()=default;

	constexpr Integer getNumerator() const;
	constexpr Integer getDenominator() const;

	constexpr bool isInteger() const;

	constexpr operator bool() const;
	constexpr operator Integer() const;
	constexpr operator Real() const;
	template<typename Q>
	constexpr operator Rational<Q>() const;

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

private:
	Integer m_num;
	Integer m_den;
};

typedef Rational<int64_t>	Rational128_t;
typedef Rational<int32_t>	Rational64_t;
typedef Rational<int16_t>	Rational32_t;
typedef Rational<int8_t>	Rational16_t;
typedef Rational128_t		Rational_t;

}

#include "Rational.inl"
