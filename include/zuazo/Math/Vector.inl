#include "Vector.h"

#include "../Utils/Functions.h"

namespace Zuazo::Math {

template<typename T, int dim>
constexpr Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	return glm::min(a, b);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	return glm::max(a, b);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi){
	 return glm::clamp(val, lo, hi);
}

template<typename T, int dim, typename Q>
constexpr Math::Vec<T, dim> lerp(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b, const Q& r) {
	return glm::mix(a, b, r);
}

template<typename T, int dim, typename Q>
constexpr Math::Vec<Q, dim> ilerp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	return static_cast<Math::Vec<Q, dim>>(val - a) / static_cast<Math::Vec<Q, dim>>(b - a);
}

template<typename T, int dim, typename Q>
constexpr Math::Vec<T, dim> map(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo1, const Math::Vec<T, dim>& hi1, const Math::Vec<T, dim>& lo2, const Math::Vec<T, dim>& hi2) {
	return lerp(lo2, hi2, ilerp<T, Q>(val, lo1, hi1));
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> mod(const Math::Vec<T, dim>& num, const Math::Vec<T, dim>& den) {
	return glm::mod(num, den);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> abs(const Math::Vec<T, dim>& v) {
	return glm::abs(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> inv(const Math::Vec<T, dim>& v) {
	return Math::Vec<T, dim>(1) / v;
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> floor(const Math::Vec<T, dim>& v) {
	return glm::floor(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> trunc(const Math::Vec<T, dim>& v) {
	return glm::trunc(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> ceil(const Math::Vec<T, dim>& v) {
	return glm::ceil(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> round(const Math::Vec<T, dim>& v) {
	return glm::round(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> frac(const Math::Vec<T, dim>& v) {
	return v - trunc(v);
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> gcd(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	constexpr Math::Vec<T, dim> result;
	for(int i = 0; i < dim; i++) {
		result[i] = Math::gcd(a[i], b[i]);
	}
	return result;
}

template<typename T, int dim>
constexpr Math::Vec<T, dim> lcm(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	constexpr Math::Vec<T, dim> result;
	for(int i = 0; i < dim; i++) {
		result[i] = Math::lcm(a[i], b[i]);
	}
	return result;
}

}