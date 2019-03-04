#pragma once

#include <bits/stdint-intn.h>
#include <math.h>
#include <stddef.h>
#include <sys/types.h>
#include <numeric>

namespace Zuazo::Utils{


struct Rational{
	int64_t num;
	int64_t den;

	constexpr Rational():
			num(0),
			den(0)
	{

	}

	constexpr Rational(int64_t num, int64_t den):
			num(num),
			den(den)
	{
		simplify();
	}

	constexpr Rational(double number);

	constexpr Rational(const Rational& rat)=default;


	constexpr operator bool() const{
		return num;
	}

	constexpr operator int() const{
		return num/den;
	}

	constexpr operator double() const{
		if(den)
			return (double)num / den;
		else if(num > 0)
			return std::numeric_limits<double>::max();
		else if(num < 0)
			return std::numeric_limits<double>::min();
		else
			return NAN;
	}

	constexpr int operator==(const Rational& right)const{
    	return operator double() == right.operator double();
    }

	constexpr int operator!=(const Rational& right)const{
    	return operator double() != right.operator double();
    }

	constexpr int operator<(const Rational& right)const{
    	return operator double() < right.operator double();
    }

	constexpr int operator<=(const Rational& right)const{
    	return operator double() <= right.operator double();
    }

	constexpr int operator>(const Rational& right)const{
    	return operator double() > right.operator double();
    }

	constexpr int operator>=(const Rational& right)const{
    	return operator double() >= right.operator double();
    }

    /*
     * operator*
     */

	constexpr Rational operator*(const Rational& right){
    	return Rational(num*right.num, den*right.den);
    }

	constexpr Rational operator*(int right){
    	return Rational(num*right, den);
    }

    friend constexpr Rational operator*(int left, const Rational& right){
    	return Rational(left * right.num, right.den);
    }

    constexpr Rational operator*(double right){
    	return (*this) * Rational(right);
    }

    friend constexpr Rational operator*(double left, const Rational& right){
    	return Rational(left) * right;
    }

    /*
     * operator/
     */

    constexpr Rational operator/(const Rational& right){
    	return Rational (num*right.den, den*right.num);
    }

    friend constexpr Rational operator/(int left, const Rational& right){
    	return Rational(left * right.den, right.num);
    }

    constexpr Rational operator/(int right){
    	return Rational(num, den*right);
    }

    constexpr Rational operator/(double right){
    	return (*this) / Rational(right);
    }

    friend constexpr Rational operator/(double left, const Rational& right){
    	return Rational(left) / right;
    }

private:
    constexpr void simplify();
};

constexpr Rational::Rational(double number) : Rational() {
	/*
	 * This code is based on:
	 * https://rosettacode.org/wiki/Convert_decimal_number_to_rational
	 */
	constexpr size_t precision=64;
	constexpr int32_t MAX_DENOMINATOR=4096;
	int64_t h[3] = { 0, 1, 0 }, k[3] = { 1, 0, 0 };
	int64_t n=1, d=0;
	bool neg = false;

	if (number < 0) {
		neg = true;
		number = -number;
	}

	while (number != floor(number)) {
		n <<= 1;
		number *= 2;
	}
	d = number;

	for (u_int32_t i = 0; i < precision; i++) {
		int64_t a = n ? d / n : 0;
		if (i && !a)
			break;

		int64_t x = d;
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

	den = k[1];
	num = neg ? -h[1] : h[1];
}

constexpr void Rational::simplify(){
	int64_t gcd=std::gcd(num, den);
	if(gcd){
		num/=gcd;
		den/=gcd;
	}
}

}
