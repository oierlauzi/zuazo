#include "Spline.h"

#include "Combinatorics.h"
#include "Exponential.h"
#include "Polynomial.h"

#include <cassert>

namespace Zuazo::Math {

/*
 * Spline
 */

template<typename T, size_t Deg>
template<typename... Values>
constexpr Spline<T, Deg>::Spline(Values&&... values)
	: m_values{ std::forward<Values>(values)... }
{
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Spline<T, Deg>::Spline(const Spline<Q, Deg>& other) {
	*this = other; //Defined below
}

template<typename T, size_t Deg>
template<typename Q>
constexpr Spline<T, Deg>& Spline<T, Deg>::operator=(const Spline<Q, Deg>& other) {
	static_assert(size() == other.size(), "Sizes must match");

	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}
}



template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type& Spline<T, Deg>::operator[](size_t i) noexcept {
	assert(i < m_values.size());
	return m_values[i];
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type& Spline<T, Deg>::operator[](size_t i) const noexcept {
	assert(i < m_values.size());
	return m_values[i];
}



template<typename T, size_t Deg>
template<typename Q>
constexpr typename Spline<T, Deg>::value_type Spline<T, Deg>::sample(Q t) const {
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
std::array<typename Spline<T, Deg>::value_type, Deg+1> Spline<T, Deg>::getPolynomialCoefficients() const {
	std::array<value_type, size()> result = {};

	//FIXME this algorithm has been obtained and tested based on the pattern 
	//of coefficients upto degree 3. I haven't done any further testing, so
	//results for higher orders might be wrong

	/* Non recursive way:
	for(size_t i = 0; i < result.size(); ++i) {
		const value_type w0(binomialCoefficient(degree(), i));
		for(size_t j = 0; j <= i; ++j) {
			const value_type sign(((i+j) % 2) ? +1 : -1);
			const value_type w1(binomialCoefficient(i, j));
			result[i] += sign * w0 * w1 * (*this)[j];
		}
	}
	*/

	if constexpr (degree() > 0) {
		//Obtain the lower degree result and coefficients
		const auto& prevSpline = reinterpret_cast<const Spline<value_type, degree()-1>&>(*this);
		auto& prevResult = reinterpret_cast<std::array<value_type, prevSpline.size()>&>(result);
		
		//Obtain the lower degree coefficients
		prevResult = prevSpline.getPolynomialCoefficients();

		//Previous results have been multiplied by binomialCoefficient(n-1, k), we need them 
		//multiplied by binomialCoefficient(n, k)
		//
		//  n!         (n-1)!           n(n-1)!      (n-1)!             n
		//-------- = ---------- x => ------------- = --------x => x = ----
		//k!(n-k)!   k!(n-1-k)!      (n-k)(n-k-1)!   (n-k-1)!          n-k
		//
		// So, multiply the previous result by n/(n-k) to get the actual value. With higher
		//degrees this might lead to numerical inestability, but it is expected to use 3rd
		//order at most.

		for(size_t i = 0; i < prevResult.size(); ++i) {
			prevResult[i] *= value_type(degree()) / value_type(degree() - i);
		}	
	}

	//Statically generate the binomial coefficients
	constexpr auto coefficients = generateBinomialCoefficients<size_t, degree()>();
	static_assert(coefficients.size() == size(), "Binomial coefficients must have the same size as this");

	//Set the last row of the resulting coefficients
	for(size_t i = 0; i < size(); ++i) {
		//binomialCoefficients(n, n) = 1, so it is not considered
		const value_type sign(((degree()+i) % 2) ? +1 : -1);
		result.back() += sign * value_type(coefficients[i]) * back();
	}
}


template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type& Spline<T, Deg>::front() noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type& Spline<T, Deg>::front() const noexcept {
	return m_values.front();
}

template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type& Spline<T, Deg>::back() noexcept {
	return m_values.back();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type& Spline<T, Deg>::back() const noexcept {
	return m_values.back();
}


template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type* Spline<T, Deg>::begin() noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type* Spline<T, Deg>::begin() const noexcept {
	return data();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type* Spline<T, Deg>::cbegin() const noexcept {
	return begin();
}

template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type* Spline<T, Deg>::end() noexcept {
	return data() + size();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type* Spline<T, Deg>::end() const noexcept {
	return data() + size();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type* Spline<T, Deg>::cend() const noexcept {
	return end();
}


template<typename T, size_t Deg>
constexpr typename Spline<T, Deg>::value_type* Spline<T, Deg>::data() noexcept {
	return m_values.data();
}

template<typename T, size_t Deg>
constexpr const typename Spline<T, Deg>::value_type* Spline<T, Deg>::data() const noexcept {
	return m_values.data();
}


template<typename T, size_t Deg>
constexpr size_t Spline<T, Deg>::size() noexcept {
	return degree() + 1;
}

template<typename T, size_t Deg>
constexpr size_t Spline<T, Deg>::degree() noexcept {
	return Deg;
}


template<typename T, size_t N>
constexpr Spline<T, N-1> derivate(const Spline<T, N>& s) {
	//Based on:
	//https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/bezier-der.html
	Spline<T, N-1> result;

	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = s.degree() * (s[i+1] - s[i]);
	}

	return result;
}

template<typename T, size_t N>
std::array<typename Spline<T, N>::value_type, 2> getBoundaries(const Spline<T, N>& s) {
	//Start with the minimum and maximum edge values
	std::array<typename Spline<T, N>::value_type, 2> result = {
		min(s.front(), s.back()), //front
		max(s.front(), s.back()) //back
	};

	//Obtain the derivate to guess the maximum and the minumum
	const auto ddt = derivate(s);

	//Obtain the roots of the derivate
	const auto roots = polynomialSolve(ddt.getCoefficients());

	//Update the minimum. Don't consider if solutions are
	//valid, as invalid solutions are set to zero, which when
	//sampled become the edge value. This makes it compatible
	//with vector semantics. Also note that in this last scenario,
	//a vector will be used as a sample parameter. This is also
	//OK, as we care about the axes independenty
	for(size_t i = 0; i < roots.size(); ++i) {
		const auto relativeMaxMin = s.sample(roots[i]);
		result.front() = min(result.front(), relativeMaxMin);
		result.back() = max(result.back(), relativeMaxMin);
	}

	result result;
}


/*
 * SplineLoop
 */

template<typename T, size_t Deg>
inline SplineLoop<T, Deg>::SplineLoop(Utils::BufferView<const value_type> values) 
	: m_values((values.size() / degree() > 0) ? (values.size() / degree() + 1) : 0)
{
	//This value is not possible!
	assert(m_values.size() != 1);

	//Fill the array
	if(m_values.size() > 0) {
		for(size_t i = 0; i < m_values.size() - 1; ++i) {
			for(size_t j = 0; j < degree(); ++j) {
				m_values[i][j] = values[i*degree() + j];
			}
		}

		//Repeat the first element at the back, so that it loops
		//over to the first value when casting to Spline
		m_values.back() = m_values.front();
	}
}

template<typename T, size_t Deg>
inline SplineLoop<T, Deg>::SplineLoop(Utils::BufferView<const Spline> splines) 
	: m_values((splines.size() > 0) ? (splines.size() + 1) : 0)
{
	//This value is not possible!
	assert(m_values.size() != 1);

	//Fill the array
	if(m_values.size() > 0) {
		for(size_t i = 0; i < m_values.size() - 1; ++i) {
			//Ensure continuity
			assert(splines[i].back() == splines[(i+1) % splines.size()].front());

			//Last value will be discarded due to continuity
			for(size_t j = 0; j < degree(); ++j) {
				m_values[i][j] = splines[i][j];
			}
		}

		//Repeat the first element at the back, so that it loops
		//over to the first value when casting to Spline
		m_values.back() = m_values.front();
	}
}

template<typename T, size_t Deg>
inline void SplineLoop<T, Deg>::setSpline(size_t i, const Spline& s) {
	assert(i < splineCount());
	reinterpret_cast<Spline&>(m_values[i*degree()]) = s;

	//Ensure continuity
	if(i == 0) {
		m_values.back() = m_values.front();
	}
}

template<typename T, size_t Deg>
inline const typename SplineLoop<T, Deg>::Spline& SplineLoop<T, Deg>::getSpline(size_t i) const noexcept {
	assert(i < splineCount());
	return reinterpret_cast<const Spline&>(m_values[i*degree()]);
}


template<typename T, size_t Deg>
template<typename Q>
inline typename SplineLoop<T, Deg>::value_type SplineLoop<T, Deg>::sample(Q t) const {
	//Sanitize the values
	const auto index = static_cast<intmax_t>(floor(t)); //Floors
	t -= index; //Obtain only the fractional part: [0, 1)
	const auto i = static_cast<size_t>((index % splineCount() + splineCount()) % splineCount()); //Index in [0, splineCount)

	//Ensure that the values are correct
	assert(Q(0) <= t && t < Q(1));
	assert(0U <= i && i < splineCount());

	//Sample
	return getSpline(i).sample(t);
}


template<typename T, size_t Deg>
inline const typename SplineLoop<T, Deg>::value_type* SplineLoop<T, Deg>::data() {
	return m_values.data();
}

template<typename T, size_t Deg>
inline size_t SplineLoop<T, Deg>::size() const {
	//Account the repeated last value
	return (m_values.size() > 0) ? (m_values.size() - 1) : 0;
}

template<typename T, size_t Deg>
inline size_t SplineLoop<T, Deg>::splineCount() const {
	return size() / degree();
}


template<typename T, size_t Deg>
constexpr size_t SplineLoop<T, Deg>::degree() {
	return Deg;
}

}