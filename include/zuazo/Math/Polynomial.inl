#include "Polynomial.h"

#include "Exponential.h"

#include <cassert>

namespace Zuazo::Math {

template<typename T, size_t Deg>
template<typename... Values>
constexpr Polynomial<T, Deg>::Polynomial(Values&&... values)
	: m_values{ std::forward<Values>(values)... }
{
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Polynomial<T, Deg>::Polynomial(const Polynomial<Q, Deg>& other) {
	*this = other; //Defined below
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Polynomial<T, Deg>& Polynomial<T, Deg>::operator=(const Polynomial<Q, Deg>& other) {
	static_assert(size() == other.size(), "Sizes must match");

	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}
}



template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::operator[](size_t i) noexcept {
	assert(i < m_values.size());
	return m_values[i];
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::operator[](size_t i) const noexcept {
	assert(i < m_values.size());
	return m_values[i];
}



template<typename T, size_t Deg>
template<typename Q>
constexpr typename Polynomial<T, Deg>::value_type Polynomial<T, Deg>::operator()(Q t) const {
	value_type result(0);

	for(size_t i = 0; < size(); ++i) {
		result += (*this)[i] * pow(t, Q(i));
	}

	return result;
}




template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::front() noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::front() const noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::back() noexcept {
	return m_values.back();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type& Polynomial<T, Deg>::back() const noexcept {
	return m_values.back();
}


template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::begin() noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::begin() const noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::cbegin() const noexcept {
	return begin();
}

template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::end() noexcept {
	return data() + size();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::end() const noexcept {
	return data() + size();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::cend() const noexcept {
	return end();
}


template<typename T, size_t Deg>
constexpr typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::data() noexcept {
	return m_values.data();
}

template<typename T, size_t Deg>
constexpr const typename Polynomial<T, Deg>::value_type* Polynomial<T, Deg>::data() const noexcept {
	return m_values.data();
}


template<typename T, size_t Deg>
constexpr size_t Polynomial<T, Deg>::size() noexcept {
	return degree() + 1;
}

template<typename T, size_t Deg>
constexpr size_t Polynomial<T, Deg>::degree() noexcept {
	return Deg;
}



template<typename T, size_t Deg>
constexpr Polynomial<T, Deg-1> derivate(const Polynomial<T, Deg>& p) {
	Polynomial<T, Deg-1> result;

	//Copy the coefficients to the destination polynomial, lowering their exponent
	//by one and multiplying them by their exponent
	for(size_t i = 1; i < p.size(); ++i) {
		result[i-1] = typename Polynomial<T, Deg>::value_type(i) * p[i];
	}

	return result;
}


template<typename T>
constexpr std::array<T, 0> solve(const Polynomial<T, 0>& poly, PolynomialSolutionCount* cnt) {
	std::array<T, 0> result = {};

	if(poly[0] == static_cast<typename Polynomial<T, 0>::value_type>(0)) {
		if(cnt) *cnt = PolynomialSolutionCount::INFINITE;
	} else {
		if(cnt) *cnt = PolynomialSolutionCount::NONE;
	}

	return result;
}

template<typename T>
constexpr std::array<T, 1> solve(const Polynomial<T, 1>& poly, PolynomialSolutionCount* cnt) {
	std::array<T, 1> result = {};

	if(poly.back() != static_cast<typename Polynomial<T, 1>::value_type>(0)) {
		result[0] = -poly[0] / poly[1];
		if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(1);
	} else {
		//Highest order coefficient is zero. Fall back into a lower order solver
		Polynomial<T, poly.degree()-1> prevPoly;
		for(size_t i = 0; i < prevPoly.size(); ++i) {
			prevPoly[i] = poly[i];
		}

		const auto prevResult = solve(prevPoly, cnt);

		for(size_t i = 0; i < prevResult.size(); ++i) {
			result[i] = prevResult[i];
		}
	}

	return result;
}

template<typename T>
constexpr std::array<T, 2> solve(const Polynomial<T, 2>& poly, PolynomialSolutionCount* cnt) {
	std::array<T, 2> result = {};

	if(poly.back() != static_cast<typename Polynomial<T, 2>::value_type>(0)) {
		//Compute the discriminant (b^2 - 4ac)
		const auto delta = poly[1]*poly[1] - static_cast<T>(4)*poly[2]*poly[0];

		if(delta < static_cast<typename Polynomial<T, 2>::value_type>(0)) {
			//No solution!
			if(cnt) *cnt = PolynomialSolutionCount::NONE;
		} else {
			//1 or 2 solutions
			const auto den = static_cast<typename Polynomial<T, 2>::value_type>(2) * poly[2]; //2a

			if(delta == static_cast<typename Polynomial<T, 2>::value_type>(0)) {
				//Only one solution
				result[0] = -poly[1] / den; //-b/2a
				if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(1);
			} else {
				//2 solutions
				result[0] = (-poly[1] + sqrt(delta)) / den; //-b + sqrt(...)/2a
				result[1] = (-poly[1] - sqrt(delta)) / den; //-b - sqrt(...)/2a
				if(cnt)	*cnt = static_cast<PolynomialSolutionCount>(2);
			}
		}
	} else {
		//Highest order coefficient is zero. Fall back into a lower order solver
		Polynomial<T, poly.degree()-1> prevPoly;
		for(size_t i = 0; i < prevPoly.size(); ++i) {
			prevPoly[i] = poly[i];
		}

		const auto prevResult = solve(prevPoly, cnt);
		
		for(size_t i = 0; i < prevResult.size(); ++i) {
			result[i] = prevResult[i];
		}
	}

	return result;
}


template<typename T, size_t N, size_t Deg>
constexpr std::array<Vec<T, N>, Deg> solve(const Polynomial<Vec<T, N>, Deg>& poly, Vec<PolynomialSolutionCount, N>* cnt = nullptr) noexcept {
	std::array<Vec<T, N>, poly.degree()-1> result;

	//Majorness needs to be modified
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		//Obtain a row of coefficients
		Polynomial<typename Vec<T, N>::value_type, poly.degree()> scalarPoly = {};
		static_assert(scalarPoly.size() == poly.size(), "Sizes must match");
		for(size_t j = 0; j < scalarPoly.size(); ++j) {
			scalarPoly[j] = poly[j][i];
		}

		//Obtain a row of results and copy it to the destination array
		const auto scalarResult = solve(scalarPoly, cnt ? &((*cnt)[i]) : nullptr);
		static_assert(scalarResult.size() == result.size(), "Sizes must match");
		for(size_t j = 0; j < result.size(); ++j) {
			result[j][i] = scalarResult[j];
		}
	}

	return result;
}

}