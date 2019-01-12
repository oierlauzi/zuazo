#pragma once

#include <sys/types.h>
#include <math.h>
#include <limits>

namespace Zuazo{

#define CAN_CAST_TO_INT(a) ( \
		a <= std::numeric_limits<int32_t>::max() && \
		a >= std::numeric_limits<int32_t>::lowest())

#define IS_FINITE_NUMBER(a) ( \
		a < std::numeric_limits<double>::infinity() && \
		a > -std::numeric_limits<double>::infinity())

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
			int64_t d=1.0;

			num=(int)number;
			den=(int)d;

			while(number != floor(number)){
				d*=10;
				number*=10;

				if(CAN_CAST_TO_INT(d) && CAN_CAST_TO_INT(number)){
					num=(int)number;
					den=(int)d;
				}else break; //Can't convert it into a fraction without losing precision
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
    	return operator bool() == right.operator double();
    }

    int operator!=(const Rational& right)const{
    	return operator bool() != right.operator double();
    }

    int operator<(const Rational& right)const{
    	return operator bool() < right.operator double();
    }

    int operator<=(const Rational& right)const{
    	return operator bool() <= right.operator double();
    }

    int operator>(const Rational& right)const{
    	return operator bool() > right.operator double();
    }

    int operator>=(const Rational& right)const{
    	return operator bool() >= right.operator double();
    }

    Rational operator*(const Rational& right){
    	Rational r(num*right.num, den*right.den);
    	r.simplify();
    	return r;
    }

    Rational operator/(const Rational& right){
    	Rational r(num*right.den, den*right.num);
    	r.simplify();
    	return r;
    }

    Rational operator*(double right){
    	return (*this) * Rational(right);
    }

    Rational operator/(double right){
    	return (*this) / Rational(right);
    }


	void simplify();
};

inline void Rational::simplify(){
	if(den && num){
		//Calcualte GCD
		//Used algorithm: https://www.programiz.com/c-programming/examples/hcf-gcd
		int32_t n1=num, n2=den;
		while(n1!=n2){
			if(n1 > n2)
				n1 -= n2;
			else
				n2 -= n1;
		}

		//Simplify the fraction
		num /= n1;
		den /= n1;
	}else if(den){
		num=0;
		den=1;
	}else if(num){
		num=num>0 ? 1 : -1;
		den=0;
	}
}
}
