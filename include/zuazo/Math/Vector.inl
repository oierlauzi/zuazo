#include "Vector.h"

#include <algorithm>
#include <utility>
#include <cassert>

namespace Zuazo::Math {

/*
 * Vec2
 */

template<typename T>
constexpr Vec<T, 2>::Vec(value_type val)
	: Vec(val, std::move(val))
{
}

template<typename T>
constexpr Vec<T, 2>::Vec(value_type x, value_type y)
	: x(std::move(x))
	, y(std::move(y))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 2>::Vec(const Vec<Q, 2>& other)
	: Vec(other.x, other.y)
{
}



template<typename T>
template<typename Q>
constexpr Vec<T, 2>& Vec<T, 2>::operator=(const Vec<Q, 2>& other) {
	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}

	return *this;
}



template<typename T>
constexpr typename Vec<T, 2>::value_type& Vec<T, 2>::operator[](size_t i) {
	assert(i < size());
	return data()[i];
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type& Vec<T, 2>::operator[](size_t i) const {
	assert(i < size());
	return data()[i];
}


template<typename T>
constexpr typename Vec<T, 2>::value_type* Vec<T, 2>::begin() {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type* Vec<T, 2>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type* Vec<T, 2>::cbegin() const {
	return data();
}

template<typename T>
constexpr typename Vec<T, 2>::value_type* Vec<T, 2>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type* Vec<T, 2>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type* Vec<T, 2>::cend() const {
	return end();
}


template<typename T>
constexpr typename Vec<T, 2>::value_type* Vec<T, 2>::data() {
	return &x;
}

template<typename T>
constexpr const typename Vec<T, 2>::value_type* Vec<T, 2>::data() const {
	return &x;
}

template<typename T>
constexpr size_t Vec<T, 2>::size() {
	return 2;
}

/*
 * Vec3
 */

template<typename T>
constexpr Vec<T, 3>::Vec(value_type val)
	: Vec(val, val, std::move(val))
{
}

template<typename T>
constexpr Vec<T, 3>::Vec(value_type x, value_type y, value_type z)
	: x(std::move(x))
	, y(std::move(y))
	, z(std::move(z))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 3>::Vec(const Vec<Q, 3>& other)
	: Vec(other.x, other.y, other.z)
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 3>::Vec(const Vec<Q, 2>& xy, value_type z)
	: Vec(xy.x, xy.y, std::move(z))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 3>::Vec(value_type x, const Vec<Q, 2>& yz)
	: Vec(std::move(x), yz.x, yz.y)
{
}


template<typename T>
template<typename Q>
constexpr Vec<T, 3>& Vec<T, 3>::operator=(const Vec<Q, 2>& other) {
	return (*this) = Vec<Q, 3>(other);
}

template<typename T>
template<typename Q>
constexpr Vec<T, 3>& Vec<T, 3>::operator=(const Vec<Q, 3>& other) {
	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}

	return *this;
}


template<typename T>
constexpr typename Vec<T, 3>::value_type& Vec<T, 3>::operator[](size_t i) {
	assert(i < size());
	return data()[i];
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type& Vec<T, 3>::operator[](size_t i) const {
	assert(i < size());
	return data()[i];
}


template<typename T>
constexpr typename Vec<T, 3>::value_type* Vec<T, 3>::begin() {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type* Vec<T, 3>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type* Vec<T, 3>::cbegin() const {
	return data();
}

template<typename T>
constexpr typename Vec<T, 3>::value_type* Vec<T, 3>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type* Vec<T, 3>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type* Vec<T, 3>::cend() const {
	return end();
}


template<typename T>
constexpr typename Vec<T, 3>::value_type* Vec<T, 3>::data() {
	return &x;
}

template<typename T>
constexpr const typename Vec<T, 3>::value_type* Vec<T, 3>::data() const {
	return &x;
}

template<typename T>
constexpr size_t Vec<T, 3>::size() {
	return 3;
}


/*
 * Vec4
 */

template<typename T>
constexpr Vec<T, 4>::Vec(value_type val)
	: Vec(val, val, val, std::move(val))
{
}

template<typename T>
constexpr Vec<T, 4>::Vec(value_type x, value_type y, value_type z, value_type w)
	: x(std::move(x))
	, y(std::move(y))
	, z(std::move(z))
	, w(std::move(w))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(const Vec<Q, 4>& other)
	: Vec(other.x, other.y, other.z, other.w)
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(const Vec<Q, 3>& xyz, value_type w)
	: Vec(xyz.x, xyz.y, xyz.z, std::move(w))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(value_type x, const Vec<Q, 3>& yzw)
	: Vec(std::move(x), yzw.x, yzw.y, yzw.z)
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(const Vec<Q, 2>& xy, value_type z, value_type w)
	: Vec(xy.x, xy.y, std::move(z), std::move(w))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(value_type x, const Vec<Q, 2>& yz, value_type w)
	: Vec(std::move(x), yz.x, yz.y, std::move(w))
{
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>::Vec(value_type x, value_type y, const Vec<Q, 2>& zw)
	: Vec(std::move(x), std::move(y), zw.x, zw.y)
{
}

template<typename T>
template<typename Q, typename R>
constexpr Vec<T, 4>::Vec(const Vec<Q, 2>& xy, const Vec<R, 2>& zw)
	: Vec(xy.x, xy.y, zw.x, zw.y)
{
}


template<typename T>
template<typename Q>
constexpr Vec<T, 4>& Vec<T, 4>::operator=(const Vec<Q, 2>& other) {
	return (*this) = Vec<Q, 4>(other);
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>& Vec<T, 4>::operator=(const Vec<Q, 3>& other) {
	return (*this) = Vec<Q, 4>(other);
}

template<typename T>
template<typename Q>
constexpr Vec<T, 4>& Vec<T, 4>::operator=(const Vec<Q, 4>& other) {
	for(size_t i = 0; i < size(); ++i) {
		(*this)[i] = other[i];
	}

	return *this;
}


template<typename T>
constexpr typename Vec<T, 4>::value_type& Vec<T, 4>::operator[](size_t i) {
	assert(i < size());
	return data()[i];
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type& Vec<T, 4>::operator[](size_t i) const {
	assert(i < size());
	return data()[i];
}


template<typename T>
constexpr typename Vec<T, 4>::value_type* Vec<T, 4>::begin() {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type* Vec<T, 4>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type* Vec<T, 4>::cbegin() const {
	return data();
}

template<typename T>
constexpr typename Vec<T, 4>::value_type* Vec<T, 4>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type* Vec<T, 4>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type* Vec<T, 4>::cend() const {
	return end();
}


template<typename T>
constexpr typename Vec<T, 4>::value_type* Vec<T, 4>::data() {
	return &x;
}

template<typename T>
constexpr const typename Vec<T, 4>::value_type* Vec<T, 4>::data() const {
	return &x;
}

template<typename T>
constexpr size_t Vec<T, 4>::size() {
	return 4;
}


/*
 * Operators
 */

template<typename T, size_t N>
constexpr bool operator==(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	bool result = true;

	//Iterate until it becomes false
	for(size_t i = 0; (i < Vec<T, N>::size()) && result; ++i) {
		result = (lhs[i] == rhs[i]);
	}

	return result;
}

template<typename T, size_t N>
constexpr bool operator!=(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	return !operator==(lhs, rhs);
}



template<typename T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& a) {
	return a;
}


template<typename T, size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& a) {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = -a[i];
	}

	return result;
}


template<typename T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] + rhs[i];
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] - rhs[i];
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] * rhs[i];
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] * rhs;
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator*(const typename Vec<T, N>::value_type& lhs, const Vec<T, N>& rhs) noexcept {
	return rhs * lhs; //Defined above
}

template<typename T, size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] / rhs[i];
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept {
	Vec<T, N> result;

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result[i] = lhs[i] / rhs;
	}

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator/(const typename Vec<T, N>::value_type& lhs, const Vec<T, N>& rhs) noexcept {
	return Vec<T, N>(rhs) / lhs;
}


template<typename T, size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] += rhs[i];
	}

	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] -= rhs[i];
	}

	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] *= rhs[i];
	}

	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] *= rhs;
	}

	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] /= rhs[i];
	}

	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept {
	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		lhs[i] /= rhs;
	}

	return lhs;
}


/*
 * Vector operations
 */

template<typename T, size_t N>
constexpr T dot(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept {
	T result = T(0);

	for(size_t i = 0; i < Vec<T, N>::size(); ++i) {
		result += lhs[i] * rhs[i];
	}

	return result;
}

template<typename T>
constexpr Vec<T, 3> cross(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) noexcept {
	return Vec<T, 3>(
		lhs.y*rhs.z - lhs.z*rhs.y,
		lhs.z*rhs.x - lhs.x*rhs.z,
		lhs.x*rhs.y - lhs.y*rhs.x
	);
}

template<typename T, size_t N>
constexpr T length(const Vec<T, N>& a) noexcept {
	return sqrt(length2(a));
}

template<typename T, size_t N>
constexpr T length2(const Vec<T, N>& a) noexcept {
	return dot(a, a);
}

template<typename T, size_t N>
constexpr Vec<T, N> normalize(const Vec<T, N>& a) noexcept {
	return a / length(a);
}

/*
 * Absolute
 */

template<typename T, size_t N>
constexpr Vec<T, N> abs(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return abs(x);
		}
	);

	return result;
}



/*
 * min/max
 */


template<typename T, size_t N>
constexpr Vec<T, N> min(const Vec<T, N>& a, const Vec<T, N>& b) noexcept {
	Vec<T, N> result;

	std::transform(
		a.cbegin(), a.cend(), b.cbegin(), result.begin(),
		[] (const auto& a, const auto& b) -> auto {
			return min(a, b);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> max(const Vec<T, N>& a, const Vec<T, N>& b) noexcept {
	Vec<T, N> result;

	std::transform(
		a.cbegin(), a.cend(), b.cbegin(), result.begin(),
		[] (const auto& a, const auto& b) -> auto {
			return max(a, b);
		}
	);

	return result;
}


/*
 * Exponential
 */

template<typename T, size_t N>
constexpr Vec<T, N> exp(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return exp(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> exp2(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return exp2(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> exp10(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return exp10(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> log(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return log(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> log2(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return log2(x);
		}
	);

	return result;
}
template<typename T, size_t N>
constexpr Vec<T, N> log10(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return log10(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const Vec<T, N>& power) noexcept {
	Vec<T, N> result;

	std::transform(
		base.cbegin(), base.cend(), power.cbegin(), result.begin(),
		[] (const auto& base, const auto& power) -> auto {
			return pow(base, power);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> sqrt(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return sqrt(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> mod(const Vec<T, N>& num, const Vec<T, N>& den) noexcept {
	Vec<T, N> result;

	std::transform(
		num.cbegin(), num.cend(), den.cbegin(), result.begin(),
		[] (const auto& num, const auto& den) -> auto {
			return mod(num, den);
		}
	);

	return result;
}

/*
 * Rounding
 */

template<typename T, size_t N>
constexpr Vec<T, N> floor(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return floor(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> trunc(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return trunc(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> ceil(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return ceil(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> round(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return round(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> fract(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return fract(x);
		}
	);

	return result;
}



/*
 * Trigonometric
 */

template<typename T, size_t N>
constexpr Vec<T, N> cos(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return cos(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> cosh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return cosh(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> sin(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return sin(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> sinh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return sinh(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> tan(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return tan(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> tanh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return tanh(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> acos(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return acos(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> acosh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return acosh(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> asin(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return asin(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> asinh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return asinh(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return atan(x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& y, const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		y.cbegin(), y.cend(), x.cbegin(), result.begin(),
		[] (const auto& y, const auto& x) -> auto {
			return atan(y, x);
		}
	);

	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> atanh(const Vec<T, N>& x) noexcept {
	Vec<T, N> result;

	std::transform(
		x.cbegin(), x.cend(), result.begin(),
		[] (const auto& x) -> auto {
			return atanh(x);
		}
	);

	return result;
}


template<typename T, size_t N>
inline std::ostream& operator<<(std::ostream& os, const Vec<T, N>& v) {
	os << "[";

	for(size_t i = 0; i < v.size(); ++i) {
		if(i > 0) os << ", ";
		os << v[i];
	}

	return os << "]";
}

}