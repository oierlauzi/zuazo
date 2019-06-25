#include <numeric>

namespace Zuazo::Math {

template<typename T>
constexpr T abs(T n){
	return n < 0 ? -n : n;
}

template<typename T>
constexpr T gcd(T a, T b){
	T result = 0;

	#if __cplusplus > 201402L
		//Using the STL implementation if avalible
		result = std::gcd(a, b);
	#else
		//Our own implementadion, based on:
		//https://www.programiz.com/cpp-programming/examples/hcf-gcd
		if(a && b){
			if(a < 0) a = -a;
			if(b < 0) b = -b;

			while(a != b){
				if(a > b)
					a -= b;
				else
					b -= a;
			}
			result = a;
		}
	#endif

	return result;
}

template<typename T>
constexpr T lcm(T a, T b){
	T result = 0;

	#if __cplusplus > 201402L
		//Using the STL implementation if avalible
		result = std::lcm(a, b);
	#else
		//Our own implementadion.
		if(a < 0) a = -a;
		if(b < 0) b = -b;

		T hcd = gcd(a, b);
		if(hcd){
			result = a * b / hcd;
		}
	#endif

	return result;
}

}