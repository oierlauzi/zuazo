#include "Combinatorics.h"

#include <type_traits>

namespace Zuazo::Math {

template<typename T>
constexpr T factorial(const T& num) noexcept {
	static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");

	T result = (num > 1) ? num : 1;
	T mult = result - 1;

	while(mult > 1) {
		result *= mult;
		--mult;
	}

	return result;
}

template<typename T>
constexpr T binomialCoefficient(const T& n, const T& k) noexcept {
	static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");

	T result = static_cast<T>(1);

	if(k != static_cast<T>(0) && k != n) {
		//Recursive definiton
		result = binomialCoefficient(n - 1, k - 1) + binomialCoefficient(n - 1, k);
	} //else result = 1

	return result;
}

template<typename T>
constexpr T triangularNumber(const T& n) noexcept {
	//As either n or n+1 is even, the multiplication 
	//is also even, so there is no problem when dividing
	//by 2
	return n * (n+1) / 2; 
}

template<typename T, size_t N>
constexpr std::array<T, N+1> generateBinomialCoefficients() noexcept {
	std::array<T, N+1> result = {};

	constexpr auto n = N;
	const auto cent = result.size() / 2 + 1; //Center index

	for(size_t i = 0; i < cent; ++i) {
		result[i] = binomialCoefficient(static_cast<T>(n), static_cast<T>(i));
	}
	
	for(size_t i = cent; i < result.size(); ++i) {
		result[i] = result[n-i];
	}

	return result;
}

template<typename T, size_t N>
constexpr std::array<std::array<T, N+1>, N+1> generatePascalsTriangle() noexcept {
	std::array<std::array<T, N+1>, N+1> result = {};
	
	for(size_t i = 0; i < result.size(); ++i) {
		const auto cent = i / 2 + 1; //Center index

		//First element is always 1
		result[i][0] = 1;
		
		//Sum the previous row onto this for the first half.
		for(size_t j = 1; j < cent; ++j) {
			result[i][j] = result[i-1][j-1] + result[i-1][j];
		}

		//Repeat symmetrically the previous values
		for(size_t j = cent; j <= i; ++j) {
			result[i][j] = result[i][i-j]; 
		}
	}

	return result;
}

}