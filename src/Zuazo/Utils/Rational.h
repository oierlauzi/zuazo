#pragma once

#include <sys/types.h>

namespace Zuazo{
/*
 * Contains a rational number
 */
struct Rational{
	int32_t num=0;
	int32_t den=0;

	void simplify();

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
};

inline void Rational::simplify(){
	if(den){
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
	}

}
}
