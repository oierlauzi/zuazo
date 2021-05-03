#include "Quaternion.h"

#include "../Utils/Hasher.h"

namespace Zuazo::Math {

template<typename T>
constexpr Quaternion<T>::Quaternion(value_type s, 
									const Vec3<value_type>& v)
	: Quaternion(s, v.x, v.y, v.z)
{
}

template<typename T>
constexpr Quaternion<T>::Quaternion(value_type w, value_type x, value_type y, value_type z)
	: w(std::move(w))
	, x(std::move(x))
	, y(std::move(y))
	, z(std::move(z))
{
}

template<typename T>
template<typename Q>
constexpr Quaternion<T>::Quaternion(const Quaternion<Q>& other)
	: Quaternion(other.w, other.x, other.y, other.z)
{
}

template<typename T>
template<typename Q>
constexpr Quaternion<T>::Quaternion(value_type w, const Vec<Q, 3>& xyz)
	: Quaternion(std::move(w), xyz.x, xyz.y, xyz.z)
{
}


template<typename T>
template<typename Q>
constexpr Quaternion<T>& Quaternion<T>::operator=(const Quaternion<Q>& other) {
	w = other.w;
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}


template<typename T>
constexpr typename Quaternion<T>::value_type& Quaternion<T>::operator[](size_t i) {
	assert(i < size());
	return data()[i];
}

template<typename T>
constexpr const typename Quaternion<T>::value_type& Quaternion<T>::operator[](size_t i) const {
	assert(i < size());
	return data()[i];
}


template<typename T>
constexpr typename Quaternion<T>::value_type* Quaternion<T>::begin() {
	return data();
}

template<typename T>
constexpr const typename Quaternion<T>::value_type* Quaternion<T>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Quaternion<T>::value_type* Quaternion<T>::cbegin() const {
	return begin();
}

template<typename T>
constexpr typename Quaternion<T>::value_type* Quaternion<T>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Quaternion<T>::value_type* Quaternion<T>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Quaternion<T>::value_type* Quaternion<T>::cend() const {
	return end();
}

template<typename T>
constexpr typename Quaternion<T>::value_type* Quaternion<T>::data() {
	return &w;
}

template<typename T>
constexpr const typename Quaternion<T>::value_type* Quaternion<T>::data() const {
	return &w;
}

template<typename T>
constexpr size_t Quaternion<T>::size() {
	return 4;
}



template<typename T>
constexpr bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	bool result = true;

	//Iterate until it becomes false
	for(size_t i = 0; (i < Quaternion<T>::size()) && result; ++i) {
		result = (lhs[i] == rhs[i]);
	}

	return result;
}

template<typename T>
constexpr bool operator!=(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	return !operator==(lhs, rhs);
}


template<typename T>
constexpr Quaternion<T> operator+(const Quaternion<T>& a) {
	return a;
}

template<typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& a) {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = -a[i];
	}

	return result;
}


template<typename T>
constexpr Quaternion<T> operator+(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = lhs[i] + rhs[i];
	}

	return result;
}

template<typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = lhs[i] - rhs[i];
	}

	return result;
}

template<typename T>
constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = lhs[i] * rhs[i];
	}

	return result;
}

template<typename T>
constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = lhs[i] * rhs[i];
	}

	return result;
}

template<typename T>
constexpr Quaternion<T> operator*(const typename Quaternion<T>::value_type& lhs, const Quaternion<T>& rhs) noexcept {
	return rhs * lhs; //Defined above
}

template<typename T>
constexpr Quaternion<T> operator/(const Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept {
	Quaternion<T> result;

	for(size_t i = 0; i < Quaternion<T>::size(); ++i) {
		result[i] = lhs[i] / rhs[i];
	}

	return result;
}


template<typename T>
constexpr Quaternion<T>& operator+=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	for(size_t i = 0; i < lhs.size(); ++i) {
		lhs[i] += rhs[i];
	}

	return lhs;
}

template<typename T>
constexpr Quaternion<T>& operator-=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	for(size_t i = 0; i < lhs.size(); ++i) {
		lhs[i] -= rhs[i];
	}

	return lhs;
}

template<typename T>
constexpr Quaternion<T>& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	//Based on GLM
	const auto p = lhs;
	const auto q = rhs;

	lhs.w = p.w*q.w - p.x*q.x - p.y*q.y - p.z*q.z;
	lhs.x = p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y;
	lhs.y = p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z;
	lhs.z = p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x;
	return lhs;
}

template<typename T>
constexpr Quaternion<T>& operator*=(Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept {
	for(size_t i = 0; i < lhs.size(); ++i) {
		lhs[i] *= rhs;
	}

	return lhs;
}

template<typename T>
constexpr Quaternion<T>& operator/=(Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept {
	for(size_t i = 0; i < lhs.size(); ++i) {
		lhs[i] /= rhs;
	}

	return lhs;
}



template<typename T>
constexpr typename Quaternion<T>::value_type dot(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
	return lhs.w*rhs.w + lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

template<typename T>
constexpr typename Quaternion<T>::value_type length2(const Quaternion<T>& q) noexcept {
	return dot(q, q);
}

template<typename T>
constexpr typename Quaternion<T>::value_type length(const Quaternion<T>& q) noexcept {
	return sqrt(length2(q));
}

template<typename T>
constexpr Quaternion<T> normalize(const Quaternion<T>& q) noexcept {
	return q / length(q);
}



template<typename T>
constexpr Quaternion<T> rotateAbout(const Vec3<T>& v, const T& alpha, normalized_t) noexcept {
	const auto halfAlpha = alpha / 2;
	const auto c = cos(halfAlpha);
	const auto s = sin(halfAlpha);

	return Quaternion<T>(c, s*v);
}

template<typename T>
constexpr Quaternion<T> rotateAbout(const Vec3<T>& v, const T& alpha) noexcept {
	return rotateAround(normalize(v), alpha, normalized);
}



template<typename T>
constexpr Mat3x3<T> toMatrix(const Quaternion<T>& q) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	const auto& w = q.w;
	const auto& x = q.x;
	const auto& y = q.y;
	const auto& z = q.z;

	return Mat3x3<T>(
		1 - 2*(y*y + z*z), 	2*(x*y + w*z), 		2*(x*z - w*y),
		2*(x*y - w*z),		1 - 2*(x*x + z*z),	2*(y*z + w*x),
		2*(x*z + w*y),		2*(y*z - w*x),		1 - 2*(x*x + y*y)
	);
}

template<typename T>
constexpr Quaternion<T> fromMatrix(const Mat3x3<T>& m) noexcept {
	Quaternion<T> result;
	typename Quaternion<T>::value_type t;

	//Based on:
	//https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
	if(m[2][2] < 0) {
		if(m[0][0] > m[1][1]) {
			t = 1 + m[0][0] - m[1][1] - m[2][2];
			result = Quaternion<T>(m[1][2]-m[2][1],	t, 					m[0][1]+m[1][0], 	m[2][0]+m[0][2]);
		} else {
			t = 1 - m[0][0] + m[1][1] - m[2][2];
			result = Quaternion<T>(m[2][0]-m[0][2], m[0][1]+m[1][0], 	t, 					m[1][2]+m[2][1]);
		}
	} else {
		if(m[0][0] < -m[1][1]) {
			t = 1 - m[0][0] - m[1][1] + m[2][2];
			result = Quaternion<T>(m[0][1]-m[1][0], m[2][0]+m[0][2], 	m[1][2]+m[2][1], 	t);
		} else {
			t = 1 + m[0][0] + m[1][1] + m[2][2];
			result = Quaternion<T>(t, 				m[1][2]-m[2][1], 	m[2][0]-m[0][2], 	m[0][1]-m[1][0]);
		}
	}

	result /= 2*sqrt(t); 
	return result;
}


template<typename T>
constexpr Quaternion<typename Vec3<T>::value_type> fromEulerAngles(const Vec3<T>& angles) noexcept {
	return fromEulerAngles(angles.x, angles.y, angles.z);
}

template<typename T>
constexpr Quaternion<T> fromEulerAngles(const T& yaw, const T& pitch, const T& roll) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	constexpr T half = T(0.5);
	const T	cy = cos(yaw * half),
    		sy = sin(yaw * half),
    		cp = cos(pitch * half),
    		sp = sin(pitch * half),
    		cr = cos(roll * half),
    		sr = sin(roll * half);

	return Quaternion<T>(
		cr*cp*cy + sr*sp*sy,
		sr*cp*cy - cr*sp*sy,
		cr*sp*cy + sr*cp*sy,
		cr*cp*sy - sr*sp*cy
	);
}

template<typename T>
constexpr Vec3<typename Quaternion<T>::value_type> toEulerAngles(const Quaternion<T>& q) noexcept {
	return Vec3<typename Quaternion<T>::value_type>(getYaw(q), getPitch(q), getRoll(q));
}

template<typename T>
constexpr typename Quaternion<T>::value_type getYaw(const Quaternion<T>& q) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	const typename Quaternion<T>::value_type siny_cosp =     2*(q.w*q.z + q.x*q.y);
	const typename Quaternion<T>::value_type cosy_cosp = 1 - 2*(q.y*q.y + q.z*q.z);
	return atan(siny_cosp, cosy_cosp);
}

template<typename T>
constexpr typename Quaternion<T>::value_type getPitch(const Quaternion<T>& q) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	typename Quaternion<T>::value_type result;
	const typename Quaternion<T>::value_type sinp = 2*(q.w*q.y - q.z*q.x);

	//Ensure it is within the range before calling asin
    if (abs(sinp) >= 1) {
        result = std::copysign(M_PI / 2, sinp);
	} else {
        result = std::asin(sinp);
	}

	return result;
}

template<typename T>
constexpr typename Quaternion<T>::value_type getRoll(const Quaternion<T>& q) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	const typename Quaternion<T>::value_type sinr_cosp = 	 2*(q.w*q.x + q.y*q.z);
	const typename Quaternion<T>::value_type cosr_cosp = 1 - 2*(q.x*q.x + q.y*q.y);
	return atan(sinr_cosp, cosr_cosp);
}

}



namespace Zuazo::Utils {

template <typename T, typename H>
constexpr typename Hasher<Math::Quaternion<T>, H>::hash_type 
Hasher<Math::Quaternion<T>, H>::operator()(const value_type& v) const noexcept {
	return hashAccumulate(v.cbegin(), v.cend());
}

}