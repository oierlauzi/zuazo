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

template<typename T, size_t N>
constexpr std::array<T, N+1> generateBinomialCoefficients() noexcept {
	std::array<T, N+1> result = {};

	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = binomialCoefficient(static_cast<T>(N), static_cast<T>(i));
	}

	return result;
}

template<typename T, size_t N>
constexpr std::array<std::array<T, N+1>, N+1> generateBinomialCoefficientTree() noexcept {
	std::array<std::array<T, N+1>, N+1> result = {};

	for(size_t i = 0; i < result.size(); ++i) {
		for(size_t j = 0; j<= i; ++j) {
			result[i][j] = binomialCoefficient(i, j);
		}
	}


	return result;
}

}