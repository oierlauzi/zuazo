#pragma once

#include <boost/integer/common_factor_rt.hpp>
#include <math.h>
#include <cstdint>
#include <limits>

namespace Zuazo::Utils{

#define IS_FINITE_NUMBER(a) ( \
		a < std::numeric_limits<double>::infinity() && \
		a > -std::numeric_limits<double>::infinity())

#define CAN_CAST_TO_INT(a) ( \
		a < std::numeric_limits<int32_t>::max() && \
		a > std::numeric_limits<int32_t>::min())

/*
 * Contains a rational number
 */
struct Rational{
	int32_t num;
	int32_t den;

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

			do{
				if(CAN_CAST_TO_INT(d) && CAN_CAST_TO_INT(number)){
					num=(int32_t)number;
					den=(int32_t)d;
				}else break;

				number*=2;
				d*=2;
			}while(number != trunc(number));

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
		int32_t gcd=boost::integer::gcd(num, den);

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
