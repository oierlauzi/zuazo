#include "Vector.h"

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/vector_relational.hpp>
#include <glm/trigonometric.hpp>

namespace Zuazo::Math {

/*
 * Absolute
 */

template<typename T, int dim>
inline Math::Vec<T, dim> abs(const Math::Vec<T, dim>& x) noexcept {
	return glm::abs(x);
}


/*
 * Comparisons
 */

template<typename T, int dim>
constexpr Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept {
	return glm::min(a, b);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept {
	return glm::max(a, b);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi) noexcept {
	return glm::clamp(val, lo, hi);
}

template<typename T, int dim>
constexpr bool isInRange(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi) noexcept {
	const auto cond1 = glm::lessThanEqual(lo, val);
	const auto cond2 = glm::lessThanEqual(val, hi);
	return glm::all(cond1) && glm::all(cond2);
}


/*
 * Exponential
 */

template<typename T, int dim>
inline Math::Vec<T, dim> exp(const Math::Vec<T, dim>& x) noexcept {
	return glm::exp(x);
}

template<typename T, int dim>
inline Math::Vec<T, dim> exp2(const Math::Vec<T, dim>& x) noexcept {
	return glm::exp2(x);
}

template<typename T, int dim>
inline Math::Vec<T, dim> exp10(const Math::Vec<T, dim>& x) noexcept {
	return Math::pow(Math::Vec<T, dim>(10), x);
}

template<typename T, int dim>
inline Math::Vec<T, dim> log(const Math::Vec<T, dim>& x) noexcept {
	return glm::log(x);
}

template<typename T, int dim>
inline Math::Vec<T, dim> log2(const Math::Vec<T, dim>& x) noexcept {
	return glm::log2(x);
}

template<typename T, int dim>
inline Math::Vec<T, dim> log10(const Math::Vec<T, dim>& x) noexcept {
	return Math::log(x) / Math::log(Math::Vec<T, dim>(10));
}

template<typename T, int dim>
inline Math::Vec<T, dim> pow(const Math::Vec<T, dim>& base, const Math::Vec<T, dim>& power) noexcept {
	return glm::pow(base, power);
}

template<typename T, int dim>
inline Math::Vec<T, dim> sqrt(const Math::Vec<T, dim>& x) noexcept {
	return glm::sqrt(x);
}


/*
 * Factor
 */

template<typename T, int dim>
inline Math::Vec<T, dim> mod(const Math::Vec<T, dim>& num, const Math::Vec<T, dim>& den) noexcept {
	return glm::mod(num, den);
}

template<typename T, int dim>
inline Math::Vec<T, dim> gcd(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept {
	Math::Vec<T, dim> result;
	
	for(int i = 0; i < dim; ++i) {
		result[i] = Math::gcd(a[i], b[i]);
	}

	return result;
}

template<typename T, int dim>
inline Math::Vec<T, dim> lcm(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept {
	Math::Vec<T, dim> result;

	for(int i = 0; i < dim; ++i) {
		result[i] = Math::lcm(a[i], b[i]);
	}

	return result;
}


/*
 * Rounding
 */

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> floor(const Math::Vec<T, dim>& x) noexcept {
	return glm::floor(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> trunc(const Math::Vec<T, dim>& x) noexcept {
	return glm::trunc(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> ceil(const Math::Vec<T, dim>& x) noexcept {
	return glm::ceil(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> round(const Math::Vec<T, dim>& x) noexcept {
	return glm::round(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> fract(const Math::Vec<T, dim>& x) noexcept {
	//return glm::fract(x);
	return x - trunc(x);
}


/*
 * Trigonometry
 */

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> cos(const Math::Vec<T, dim>& x) noexcept {
	return glm::cos(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> cosh(const Math::Vec<T, dim>& x) noexcept {
	return glm::cosh(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> sin(const Math::Vec<T, dim>& x) noexcept {
	return glm::sin(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> sinh(const Math::Vec<T, dim>& x) noexcept {
	return glm::sinh(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> tan(const Math::Vec<T, dim>& x) noexcept {
	return glm::tan(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> tanh(const Math::Vec<T, dim>& x) noexcept {
	return glm::tanh(x);
}


template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> acos(const Math::Vec<T, dim>& x) noexcept {
	return glm::acos(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> acosh(const Math::Vec<T, dim>& x) noexcept {
	return glm::acosh(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> asin(const Math::Vec<T, dim>& x) noexcept {
	return glm::asin(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> asinh(const Math::Vec<T, dim>& x) noexcept {
	return glm::asinh(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> atan(const Math::Vec<T, dim>& x) noexcept {
	return glm::atan(x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> atan(const Math::Vec<T, dim>& y, const Math::Vec<T, dim>& x) noexcept {
	return glm::atan(y, x);
}

template<typename T, int dim, typename Q>
inline Math::Vec<T, dim> atanh(const Math::Vec<T, dim>& x) noexcept {
	return glm::atanh(x);
}

}