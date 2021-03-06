#include "Bezier.h"

#include "Combinatorics.h"
#include "Comparisons.h"
#include "Exponential.h"
#include "Matrix.h"
#include "Polynomial.h"

#include <cassert>

namespace Zuazo::Math {

/*
 * Bezier
 */

template<typename T, size_t Deg>
template<typename... Values>
constexpr Bezier<T, Deg>::Bezier(Values&&... values)
	: m_values{ std::forward<Values>(values)... }
{
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Bezier<T, Deg>::Bezier(const Bezier<Q, Deg>& other) {
	*this = other; //Defined below
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Bezier<T, Deg>& Bezier<T, Deg>::operator=(const Bezier<Q, Deg>& other) {
	static_assert(size() == other.size(), "Sizes must match");

	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}
}



template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::operator[](size_t i) noexcept {
	assert(i < m_values.size());
	return m_values[i];
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::operator[](size_t i) const noexcept {
	assert(i < m_values.size());
	return m_values[i];
}



template<typename T, size_t Deg>
template<typename Q>
constexpr typename Bezier<T, Deg>::value_type Bezier<T, Deg>::operator()(const Q& t) const {
	value_type result = value_type(0);

	//Statically generate the binomial coefficients
	constexpr auto coefficients = generateBinomialCoefficients<size_t, degree()>();
	static_assert(coefficients.size() == size(), "Binomial coefficients must have the same size as this");

	//Obtain the value and complementary value of t
	const auto t0 = t;
	const auto t1 = Q(1) - t0;

	//Calculate the actual sample value according to the following formula:
	//https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Explicit_definition
	for(size_t i = 0; i < size(); ++i) {
		//Assuming that 0^0 retuns 1 and not NaN
		const auto w0 = pow(t0, static_cast<decltype(t0)>(i));
		const auto w1 = pow(t1, static_cast<decltype(t1)>(degree()-i));
		result += coefficients[i] * w0 * w1 * (*this)[i];
	}

	return result;
}



template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::front() noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::front() const noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::back() noexcept {
	return m_values.back();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type& Bezier<T, Deg>::back() const noexcept {
	return m_values.back();
}


template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::begin() noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::begin() const noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::cbegin() const noexcept {
	return begin();
}

template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::end() noexcept {
	return begin() + size();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::end() const noexcept {
	return begin() + size();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::cend() const noexcept {
	return end();
}


template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::data() noexcept {
	return m_values.data();
}

template<typename T, size_t Deg>
constexpr const typename Bezier<T, Deg>::value_type* Bezier<T, Deg>::data() const noexcept {
	return m_values.data();
}


template<typename T, size_t Deg>
inline void Bezier<T, Deg>::reverse() noexcept {
	std::reverse(
		m_values.begin(),
		m_values.end()
	);
}


template<typename T, size_t Deg>
constexpr Bezier<typename Bezier<T, Deg>::value_type, 1> Bezier<T, Deg>::getAxis() const noexcept {
	return Bezier<value_type, 1>(front(), back());
}

template<typename T, size_t Deg>
constexpr typename Bezier<T, Deg>::value_type Bezier<T, Deg>::getDelta() const noexcept {
	return back() - front();
}


template<typename T, size_t Deg>
constexpr size_t Bezier<T, Deg>::size() noexcept {
	return degree() + 1;
}

template<typename T, size_t Deg>
constexpr size_t Bezier<T, Deg>::degree() noexcept {
	return Deg;
}



template<typename T, size_t Deg>
constexpr bool operator==(const Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) noexcept {
	return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template<typename T, size_t Deg>
constexpr bool operator!=(const Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) noexcept {
	return !(lhs == rhs);
}


template<typename T, size_t Deg>
constexpr Bezier<T, Deg> operator+(const Bezier<T, Deg>& a) {
	return a;
}

template<typename T, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::negate<void>, T>::type, Deg>
operator-(const Bezier<T, Deg>& b) {
	return transform(
		std::negate<void>(),
		b
	);
}


template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::plus<void>, T, Q>::type, Deg>
operator+(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		std::plus<void>(),
		lhs, rhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::minus<void>, T, Q>::type, Deg>
operator-(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		std::minus<void>(),
		lhs, rhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		std::multiplies<void>(),
		lhs, rhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Bezier<T, Deg>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::multiplies<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const T& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::multiplies<void>()(lhs, rhs);
		},
		rhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		std::divides<void>(),
		lhs, rhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Bezier<T, Deg>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::divides<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg>
operator/(const T& lhs, const Bezier<Q, Deg>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::divides<void>()(lhs, rhs);
		},
		rhs
	);
}


template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator+=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) {
	return lhs = lhs + rhs;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator-=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) {
	return lhs = lhs - rhs;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator*=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) {
	return lhs = lhs * rhs;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator*=(Bezier<T, Deg>& lhs, const typename Bezier<T, Deg>::value_type& rhs) {
	return lhs = lhs * rhs;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator/=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) {
	return lhs = lhs / rhs;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator/=(Bezier<T, Deg>& lhs, const typename Bezier<T, Deg>::value_type& rhs) {
	return lhs = lhs / rhs;
}



template<typename Func, typename... T, size_t Deg>
constexpr Bezier<typename std::invoke_result<Func, const typename Bezier<T, Deg>::value_type&...>::type, Deg> 
transform(Func f, const Bezier<T, Deg>&... v) {
	Bezier<typename std::invoke_result<Func, const typename Bezier<T, Deg>::value_type&...>::type, Deg> result;

	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = f(v[i]...);
	}

	return result;
}



template<typename T, size_t N, size_t Deg>
constexpr void setComponent(Bezier<Vec<T, N>, Deg>& s, size_t i, const Bezier<typename Vec<T, N>::value_type, Deg>& c) {
	static_assert(s.size() == c.size(), "Sizes must match");
	for(size_t j = 0; j < s.size(); ++j) {
		s[j][i] = c[j];
	}
}

template<typename T, size_t N, size_t Deg>
constexpr Bezier<typename Vec<T, N>::value_type, Deg> getComponent(const Bezier<Vec<T, N>, Deg>& s, size_t i) {
	Bezier<typename Vec<T, N>::value_type, Deg> result;
	static_assert(s.size() == result.size(), "Sizes must match");

	for(size_t j = 0; j < result.size(); ++j) {
		result[j] = s[j][i];
	}

	return result;
}


template<typename T, size_t Deg>
constexpr Polynomial<typename Bezier<T, Deg>::value_type, Deg> toPolynomial(const Bezier<T, Deg>& s) {
	Polynomial<typename Bezier<T, Deg>::value_type, Deg> result;

	//FIXME this algorithm has been obtained and tested based on the pattern 
	//of coefficients upto degree 3. I haven't done any further testing, so
	//results for higher orders might be wrong

	constexpr auto pascalTriangle = generatePascalsTriangle<size_t, s.degree()>();
	static_assert(pascalTriangle.size() == s.size(), "Triangle must have the same size as this");
	static_assert(pascalTriangle.back().size() == s.size(), "Triangle must have the same size as this");

	for(size_t i = 0; i < result.size(); ++i) {
		//Last row in the triangle is equivalent to generateBinomialCoefficients()
		const typename Bezier<T, Deg>::value_type w0(pascalTriangle.back()[i]);

		for(size_t j = 0; j <= i; ++j) {
			//Equivalent to binomialCoefficient(i ,j)
			const typename Bezier<T, Deg>::value_type w1(pascalTriangle[i][j]);

			//Alternate sign between columns and rows
			const typename Bezier<T, Deg>::value_type sign(((i+j) % 2) ? +1 : -1);

			//Accumulate the result
			result[i] += sign * w0 * w1 * s[j];
		}
	}

	return result;
}

template<typename T, size_t Deg>
constexpr Bezier<T, Deg-1> derivate(const Bezier<T, Deg>& s) {
	//Based on:
	//https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/bezier-der.html
	Bezier<T, s.degree()-1> result;

	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = s.degree() * (s[i+1] - s[i]);
	}

	return result;
}

template<typename T, size_t Deg>
constexpr std::array<typename Bezier<T, Deg>::value_type, Deg> solve(	const Bezier<T, Deg>& s, 
																		SolutionCount* cnt) 
{
	SolutionCount backupSolutionCount;
	auto& solutionCount = cnt ? *cnt : backupSolutionCount;

	const auto polynomial = toPolynomial(s);
	auto solution = solve(polynomial, &solutionCount);

	if(solutionCount > SolutionCount::NONE) {
		//Remove all invalid solutions
		const auto ite = std::remove_if(
			solution.begin(),
			std::next(solution.begin(), static_cast<size_t>(solutionCount)),
			[] (const typename Bezier<T, Deg>::value_type& value) -> bool {
				return !isInRange(
					value, 
					typename Bezier<T, Deg>::value_type(0), 
					typename Bezier<T, Deg>::value_type(1)
				);
			}
		);

		//Update the solution count
		solutionCount = static_cast<SolutionCount>(std::distance(solution.begin(), ite));
	}

	return solution;
}

template<typename T, size_t N, size_t Deg>
constexpr std::array<typename Bezier<Vec<T, N>, Deg>::value_type, Deg> solve(	const Bezier<Vec<T, N>, Deg>& s, 
																				Vec<SolutionCount, N>* cnt )
{
	std::array<typename Bezier<Vec<T, N>, Deg>::value_type, Deg> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		//Obtain the bezier functions for each component
		const auto scalarBezier = getComponent(s, i);

		//Solve the scalar bezier and copy the solution to the result array
		const auto scalarResult = solve(scalarBezier, cnt ? &((*cnt)[i]) : nullptr);
		setComponent(result, i, scalarResult);
	}

	return result;
}


template<typename T, size_t Deg, typename Q>
constexpr std::array<Bezier<T, Deg>, 2> split(const Bezier<T, Deg>& bezier, const Q& t) noexcept {
	std::array<Bezier<T, bezier.degree()>, 2> result;

	//Set the trivial points:
	result.front().front() = bezier.front();
	result.back().back() = bezier.back();

	//Obtain the rest of the points using Casteljau's algorithm
	auto casteljauValues = bezier;
	for(size_t i = 1; i < bezier.size(); ++i) {
		const size_t j = bezier.degree()-i; //Complementary i

		for(size_t k = 0; k <= j; ++k) {
			casteljauValues[k] = lerp(casteljauValues[k], casteljauValues[k+1], t);
		}

		//Choose from the first and last values
		//for each segment
		result.front()[i] = casteljauValues[0];
		result.back()[j] = casteljauValues[j];
	}

	return result;
}


template<typename T, size_t Deg>
constexpr Utils::Range<typename Bezier<T, Deg>::value_type> getBoundaries(const Bezier<T, Deg>& s) {
	//Start with the minimum and maximum edge values
	Utils::Range<typename Bezier<T, Deg>::value_type> result(
		min(s.front(), s.back()), //front
		max(s.front(), s.back()) //back
	);

	//Obtain the derivate to guess the maximum and the minumum
	const auto ddt = derivate(s);

	//Obtain the roots of the derivate
	SolutionCount solutionCount;
	const auto roots = solve(ddt, &solutionCount);

	//Update the minimum and maximum with the valid solutions
	assert(static_cast<int>(solutionCount) <= static_cast<int>(roots.size()));
	for(int i = 0; i < static_cast<int>(solutionCount); ++i) {
		const auto relativeMaxMin = s(roots[i]);
		result.setMin(min(result.getMin(), relativeMaxMin));
		result.setMax(max(result.getMax(), relativeMaxMin));
	}

	return result;
}

template<typename T, size_t N, size_t Deg>
constexpr Utils::Range<typename Bezier<Vec<T, N>, Deg>::value_type> getBoundaries(const Bezier<Vec<T, N>, Deg>& s) {
	typename Bezier<Vec<T, N>, Deg>::value_type min, max;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		//Obtain the bezier functions for each component
		const auto scalarBezier = getComponent(s, i);

		//Solve the scalar bezier and copy the solution to the result array
		const auto scalarResult = getBoundaries(scalarBezier);
		min[i] = scalarResult.getMin();
		max[i] = scalarResult.getMax();
	}

	return Utils::Range<typename Bezier<Vec<T, N>, Deg>::value_type>(min, max);
}

}