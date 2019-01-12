#pragma once

#include <bits/stdint-intn.h>
#include <math.h>
#include <algorithm>
#include <limits>

namespace Zuazo{

#define IS_FINITE_NUMBER(a) ( \
		a < std::numeric_limits<double>::infinity() && \
		a > -std::numeric_limits<double>::infinity())

/*
 * Contains a rational number
 */
struct Rational{
	int64_t num;
	int64_t den;

	Rational(){
		num=0;
		den=0;
	}

	Rational(int32_t num, int32_t den){
		this->num=num;
		this->den=den;

		simplify();
	}

	Rational(double number){
		if(IS_FINITE_NUMBER(number)){
			int64_t d=1;

			num=(int64_t)number;
			den=(int64_t)d;

			while(number != floor(number)){
				number*=2;
				d*=2;

				num=(int64_t)number;
				den=(int64_t)d;

				if(d > std::numeric_limits<int64_t>::max()/2)
					break;
			}

			simplify();
		}else{
			num=number>0 ? 1 : -1;
			den=0;
		}
	}

	Rational(const Rational& rat)=default;


	operator bool() const{
		return num;
	}

	operator int() const{
		return num/den;
	}

	operator double() const{
		return (double)num/den;
	}

    int operator==(const Rational& right)const{
    	return operator double() == right.operator double();
    }

    int operator!=(const Rational& right)const{
    	return operator double() != right.operator double();
    }

    int operator<(const Rational& right)const{
    	return operator double() < right.operator double();
    }

    int operator<=(const Rational& right)const{
    	return operator double() <= right.operator double();
    }

    int operator>(const Rational& right)const{
    	return operator double() > right.operator double();
    }

    int operator>=(const Rational& right)const{
    	return operator double() >= right.operator double();
    }

    /*
     * operator*
     */

    Rational operator*(const Rational& right){
    	return Rational(num*right.num, den*right.den);
    }

    Rational operator*(int right){
    	return Rational(num*right, den);
    }

    friend Rational operator*(int left, const Rational& right){
    	return Rational(left * right.num, right.den);
    }

    Rational operator*(double right){
    	return (*this) * Rational(right);
    }

    friend Rational operator*(double left, const Rational& right){
    	return Rational(left) * right;
    }

    /*
     * operator/
     */

    Rational operator/(const Rational& right){
    	return Rational (num*right.den, den*right.num);
    }

    friend Rational operator/(int left, const Rational& right){
    	return Rational(left * right.den, right.num);
    }

    Rational operator/(int right){
    	return Rational(num, den*right);
    }

    Rational operator/(double right){
    	return (*this) / Rational(right);
    }

    friend Rational operator/(double left, const Rational& right){
    	return Rational(left) / right;
    }


private:
    void simplify();
};

inline void Rational::simplify(){
	if(den && num){
		//Calcualte GCD
		int64_t gcd=std::__gcd(num, den);

		//Simplify the fraction
		num /= gcd;
		den /= gcd;
	}else if(den){
		num=0;
		den=1;
	}else if(num){
		num=num>0 ? 1 : -1;
		den=0;
	}
}

}
