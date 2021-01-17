#include "Spline.h"

#include "Combinatorics.h"
#include "Exponential.h"

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
	constexpr auto binomials = generateBinomials<size_t, degree()>();
	static_assert(binomials.size() == size(), "Binomial coefficients must have the same size as this");

	//Obtain the value and complementary value of t
	const auto t0 = t; //(store as local)
	const auto t1 = 1 - t0;

	//Calculate the actual sample value according to the following formula:
	//https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Explicit_definition
	for(size_t i = 0; i < size(); ++i) {
		const auto w0 = pow(t0, static_cast<decltype(t0)>(i));
		const auto w1 = pow(t1, static_cast<decltype(t1)>(degree()-i));
		result += binomials[i] * w0 * w1 * (*this)[i];
	}

	return result;
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