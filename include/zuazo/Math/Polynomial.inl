#include "Polynomial.h"

#include "Exponential.h"

#include <cassert>

namespace Zuazo::Math {

constexpr struct DontReduce {} dontReduce;

//Constant
template<typename T>
constexpr std::array<T, 0> polynomialSolve(DontReduce, const std::array<T, 1>& coeff, PolynomialSolutionCount* cnt) noexcept {
	std::array<T, 0> result = {};

	if(cnt)	*cnt = (coeff[0] == static_cast<T>(0)) ? PolynomialSolutionCount::INFINITE : PolynomialSolutionCount::NONE;

	return result;
}

//Linear
template<typename T>
constexpr std::array<T, 1> polynomialSolve(DontReduce, const std::array<T, 2>& coeff, PolynomialSolutionCount* cnt) noexcept {
	std::array<T, 1> result = {};
	assert(coeff.back() != static_cast<T>(0));

	result[0] = -coeff[0] / coeff[1];
	if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(1);

	return result;
}

//Quadratic
template<typename T>
constexpr std::array<T, 2> polynomialSolve(DontReduce, const std::array<T, 3>& coeff, PolynomialSolutionCount* cnt) noexcept {
	std::array<T, 2> result = {};
	assert(coeff.back() != static_cast<T>(0));

	const auto inSqrt = coeff[1]*coeff[1] - static_cast<T>(4)*coeff[2]*coeff[0]; //b^2 - 4ac
	if(inSqrt < static_cast<T>(0)) {
		//No solution!
		if(cnt) *cnt = PolynomialSolutionCount::NONE;
	} else {
		//1 or 2 solutions
		const auto den = static_cast<T>(2) * coeff[2]; //2a

		if(inSqrt == static_cast<T>(0)) {
			//Only one solution
			result[0] = -coeff[1] / den; //-b/2a
			if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(1);
		} else {
			//2 solutions
			result[0] = (-coeff[1] + sqrt(inSqrt)) / den; //-b + sqrt(...)/2a
			result[1] = (-coeff[1] - sqrt(inSqrt)) / den; //-b - sqrt(...)/2a
			if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(2);
		}
	}

	return result;
}


//In order to stop the recursion of the polynomialSolve function:
template<typename T>
constexpr std::array<T, 0> polynomialSolve(const std::array<T, 1>& coeff, PolynomialSolutionCount* cnt) noexcept {
	return polynomialSolve(dontReduce, coeff, cnt);
}

template<typename T, size_t N>
inline std::array<T, N-1> polynomialSolve(const std::array<T, N>& coeff, PolynomialSolutionCount* cnt) noexcept {
	std::array<T, N-1> result = {};

	if(coeff.back() != static_cast<T>(0)) {
		result = polynomialSolve(dontReduce, coeff, cnt);
	} else {
		//Biggest order coefficient is zero, so fall back into a lower order solver
		reinterpret_cast<std::array<T, N-2>&>(result) = polynomialSolve(reinterpret_cast<const std::array<T, N-1>&>(coeff), cnt);
	}

	return result;
}


template<typename T, size_t N, size_t M>
inline std::array<Vec<T, M>, N-1> polynomialSolve(const std::array<Vec<T, M>, N>& coeff, Vec<PolynomialSolutionCount, M>* cnt) noexcept {
	std::array<Vec<T, M>, N-1> result;

	//Majorness needs to be modified
	for(size_t i = 0; i < Vec<T, M>::size(); ++i) {
		//Obtain a row of coefficients
		std::array<typename Vec<T, M>::value_type, N>& scalarCoeff;
		static_assert(scalarCoeff.size() == coeff.size(), "Sizes must match");
		for(size_t j = 0; j < scalarCoeff.size(); ++j) {
			scalarCoeff[j] = coeff[j][i];
		}

		//Obtain a row of results and copy it to the destination array
		const auto scalarResult = polynomialSolve(scalarCoeff, cnt ? &(*cnt[i]) : nullptr);
		static_assert(scalarResult.size() == result.size(), "Sizes must match");
		for(size_t j = 0; j < result.size(); ++j) {
			result[j][i] = scalarResult[j];
		}
	}

	return result;
}

}