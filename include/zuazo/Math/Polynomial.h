#pragma once

#include "Vector.h"

#include <array>
#include <cstddef>
#include <limits>

namespace Zuazo::Math {

enum class PolynomialSolutionCount {
	NONE = 0,
	INFINITE = std::numeric_limits<int>::max()
};

/**
 * @brief Solves t for c_0 + c_1*t + c_2*t^2 + ... + c_n*t^n = 0 equation
 * 
 * @tparam T Used type
 * @tparam N Number of coefficients of the polynomial: 1, 2, or 3. Also the degree of the polynomial plus one
 * @param coeff Coefficients of the polynomial
 * @param cnt Solution count. Can be nullptr.
 * @return constexpr std::array<T, N> Solutions. Only filled when the number of solutions is a non-zero finite number 
 */
template<typename T, size_t N>
std::array<T, N-1> polynomialSolve(const std::array<T, N>& coeff, PolynomialSolutionCount* cnt = nullptr) noexcept;


template<typename T, size_t N, size_t M>
std::array<Vec<T, M>, N-1> polynomialSolve(const std::array<Vec<T, M>, N>& coeff, Vec<PolynomialSolutionCount, M>* cnt = nullptr) noexcept;

}

#include "Polynomial.inl"