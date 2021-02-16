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
	return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
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
constexpr Vec<typename std::invoke_result<std::negate<void>, T>::type, N>
operator-(const Vec<T, N>& a) {
	return transform(std::negate<void>(), a);
}

template<typename T, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_not<void>, T>::type, N>
operator~(const Vec<T, N>& a) {
	return transform(std::bit_not<void>(), a);
}



template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::plus<void>, T, Q>::type, N>
operator+(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::plus<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::minus<void>, T, Q>::type, N>
operator-(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::minus<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::multiplies<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::multiplies<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::multiplies<void>()(lhs, rhs);
		},
		rhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N> 
operator/(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::divides<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N> 
operator/(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::divides<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N>
operator/(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::divides<void>()(lhs, rhs);
		},
		rhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N> 
operator%(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::modulus<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N> 
operator%(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::modulus<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N>
operator%(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::modulus<void>()(lhs, rhs);
		},
		rhs 
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N> 
operator&(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::bit_and<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N> 
operator&(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::bit_and<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N>
operator&(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::bit_and<void>()(lhs, rhs);
		},
		rhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N> 
operator|(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::bit_or<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N> 
operator|(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::bit_or<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N>
operator|(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::bit_or<void>()(lhs, rhs);
		},
		rhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N> 
operator^(const Vec<T, N>& lhs, const Vec<Q, N>& rhs) {
	return transform(std::bit_xor<void>(), lhs, rhs);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N> 
operator^(const Vec<T, N>& lhs, const Q& rhs) {
	return transform(
		[&rhs] (const auto& lhs) -> auto {
			return std::bit_xor<void>()(lhs, rhs);
		},
		lhs
	);
}

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N>
operator^(const T& lhs, const Vec<Q, N>& rhs) {
	return transform(
		[&lhs] (const auto& rhs) -> auto {
			return std::bit_xor<void>()(lhs, rhs);
		},
		rhs
	);
}



template<typename T, size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs + rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs - rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs * rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs * rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs / rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs / rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator%=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs % rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator%=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs % rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator&=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs & rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator&=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs & rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator|=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = rhs | rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator|=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs | rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator^=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
	return lhs = lhs ^ rhs;
}

template<typename T, size_t N>
constexpr Vec<T, N>& operator^=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) {
	return lhs = lhs ^ rhs;
}


/*
 * Vector operations
 */

template<typename Func, typename... T, size_t N>
constexpr Vec<typename std::invoke_result<Func, const typename Vec<T, N>::value_type&...>::type, N> 
transform(Func f, const Vec<T, N>&... v) {
	Vec<typename std::invoke_result<Func, const typename Vec<T, Deg>::value_type&...>::type, N> result;

	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = f(v[i]...);
	}

	return result;
}

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

template<typename T, size_t N>
constexpr Vec<T, N> proj(const Vec<T, N>& dir, const Vec<T, N>& p) noexcept {
	return dot(p, normalize(dir)) * dir;
}

template<typename T>
constexpr typename Vec2<T>::value_type signedDistance(const Vec2<T>& origin, const Vec2<T>& direction, const Vec2<T>& point) {
	const auto perpendicular = Vec2<T>(-direction.y, direction.x);
	return dot(perpendicular, point - origin);
}

template<typename T>
constexpr void align(Utils::BufferView<Vec<T, 2>> points) {
	//Set the origin in points[0]
	for(size_t i = 1; i < points.size(); ++i) {
		points[i] -= points[0];
	}
	points[0] = Vec<T, 2>(); //points[0] -= points[0] will result in 0

	//Apply the rotatation to it
	const auto dir = points.back(); //-points.front()
	const auto len = length(dir);
	const auto c = dir[0] / len; //cos: x
	const auto s = dir[1] / len; //sin: y

	//Rotate all the points. the first and last ones are
	//trivial.
	for(size_t i = 1; i < points.size() - 1; ++i) {
		//This is equivalent to multiplying by a 2x2 rotation
		//matrix. However, as the matrix class has not been declared
		//yet, well manually implement it. Note that the rotation 
		//matrix is defined as the following:
		// [cos(O), -sin(O); sin(O), cos(O)]
		// As we want to rotate in the opposite direction of the direction
		//vector, substituting O by -O inverts the sign of the sines.
		points[i][0] =  points[i][0]*c + points[i][1]*s;
		points[i][1] = -points[i][1]*s + points[i][1]*c;
	}
	//points.front() = Vec<T, 2>(); //Already set
	points.back() = Vec<T, 2>(len, 0);
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