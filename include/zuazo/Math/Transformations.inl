#include "Transformations.h"

#include "Trigonometry.h"

namespace Zuazo::Math {

/*
 * Most of the following functions are based on GLM source
 * code. The LH_ZO (left handed, depth 0-1) variant has been
 * chosen, as it is the one that suits Vulkan
 */

template<typename T>
constexpr Mat4x4<T> frustum(T left, 	T right, 
							T bottom, 	T top, 
							T near, 	T far ) noexcept
{
	Mat4x4<T> result(static_cast<T>(0));

	result[0][0] = (static_cast<T>(2) * near) / (right - left);
	result[1][1] = (static_cast<T>(2) * near) / (top - bottom);
	result[2][0] = (right + left) / (right - left);
	result[2][1] = (top + bottom) / (top - bottom);
	result[2][2] = far / (far - near);
	result[2][3] = static_cast<T>(1);
	result[3][2] = -(far * near) / (far - near);

	return result;
}

template<typename T>
constexpr Mat4x4<T> perspective(T fovy, 
								T aspect, 
								T near ) noexcept
{
	Mat4x4<T> result(static_cast<T>(0));
	const T range = tan(fovy / static_cast<T>(2)) * near;
	const T left = -range * aspect;
	const T right = range * aspect;
	const T bottom = -range;
	const T top = range;


	result[0][0] = (static_cast<T>(2) * near) / (right - left);
	result[1][1] = (static_cast<T>(2) * near) / (top - bottom);
	result[2][2] = static_cast<T>(1);
	result[2][3] = static_cast<T>(1);
	result[3][2] = - static_cast<T>(2) * near;

	return result;
}

template<typename T>
constexpr Mat4x4<T> perspective(T fovy, 
								T aspect, 
								T near,
								T far ) noexcept
{
	assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
	Mat4x4<T> result(static_cast<T>(0));
	const T tanHalfFovy = tan(fovy / static_cast<T>(2));

	result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
	result[1][1] = static_cast<T>(1) / (tanHalfFovy);
	result[2][2] = far / (far - near);
	result[2][3] = static_cast<T>(1);
	result[3][2] = -(far * near) / (far - near);

	return result;
}

template<typename T>
constexpr Mat4x4<T> ortho(	T left, 	T right, 
							T bottom, 	T top  ) noexcept
{
	Mat4x4<T> result(static_cast<T>(1));

	result[0][0] = static_cast<T>(2) / (right - left);
	result[1][1] = static_cast<T>(2) / (top - bottom);
	result[2][2] = - static_cast<T>(1);
	result[3][0] = - (right + left) / (right - left);
	result[3][1] = - (top + bottom) / (top - bottom);

	return result;
}

template<typename T>
constexpr Mat4x4<T> ortho(	T left, 	T right, 
							T bottom, 	T top, 
							T near, 	T far ) noexcept
{
	Mat4x4<T> result(static_cast<T>(1));

	result[0][0] = static_cast<T>(2) / (right - left);
	result[1][1] = static_cast<T>(2) / (top - bottom);
	result[2][2] = static_cast<T>(1) / (far - near);
	result[3][0] = - (right + left) / (right - left);
	result[3][1] = - (top + bottom) / (top - bottom);
	result[3][2] = - near / (far - near);

	return result;
}


template<typename T>
constexpr Mat4x4<T> lookAt(	const Vec3<T>& eye,
							const Vec3<T>& center,
							const Vec3<T>& up ) noexcept
{
	Mat4x4<T> result(static_cast<T>(0));
	const auto f = normalize(center - eye);
	const auto s = normalize(cross(up, f));
	const auto u = cross(f, s);

	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = f.x;
	result[1][2] = f.y;
	result[2][2] = f.z;
	result[3][0] = -dot(s, eye);
	result[3][1] = -dot(u, eye);
	result[3][2] = -dot(f, eye);

	return result;
}



template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> translate(const Vec<T, N>& delta) noexcept {
	Mat<T, N+1, N+1> result(T(1));

	result[result.columns()-1] = Vec<T, N+1>(delta, T(1));
}

template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> translate(const Mat<T, N+1, N+1>& m, const Vec<T, N>& delta) noexcept {
	auto result = m;

	for(size_t i = 0; i < delta.size(); ++i) {
		result[m.columns()-1] += m[i]*delta[i];
	}

	return result;
}



template<typename T, size_t N>
constexpr Mat<T, N, N> scale(const Vec<T, N>& s) noexcept {
	return Mat<T, N, N>(s); //Sets the diagonal to s
}

template<typename T, size_t N>
constexpr Mat<T, N, N> scale(const Mat<T, N, N>& m, const Vec<T, N>& s) noexcept {
	auto result = m;

	for(size_t i = 0; i < s.size(); ++i) {
		result[i] *= s[i];
	}

	return result;
}

template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> scale(const Mat<T, N+1, N+1>& m, const Vec<T, N>& s) noexcept {
	auto result = m;

	for(size_t i = 0; i < s.size(); ++i) {
		result[i] *= s[i];
	}

	return result;
}



template<typename T>
constexpr Mat2x2<T> rotate(const T& angle) noexcept {
	const auto c = cos(angle);
	const auto s = sin(angle);

	//Create a 2D rotation matrix
	return Mat2x2<T>(
		+c, -s,
		+s,	+c
	);
}

template<typename T>
constexpr Mat2x2<T> rotate(const Mat2x2<T>& m, const T& angle) noexcept {
	return rotate(angle) * m;
}

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const T& angle) noexcept {
	const auto r = rotate(angle);

	//Multiply the rotation matrix with the input matrix. Manually done to improve performance
	return Mat3x3<T>(
		m[0]*r[0][0] + m[1]*r[0][1],
		m[0]*r[1][0] + m[1]*r[1][1],
		m[2]
	);
}

template<typename T>
constexpr Mat3x3<T> rotate(const Vec3<T>& axis, const T& angle) noexcept {
	const auto c = cos(angle);
	const auto s = sin(angle);
	const auto v = normalize(axis);
	const auto temp = (static_cast<T>(1) - c) * axis;

	return Mat3x3<T>(
		c + temp[0] * v[0], 		/**/ temp[1] * v[0] - s * v[2],	/**/ temp[2] * v[0] + s * v[1],
		temp[0] * v[1] + s * v[2],	/**/ c + temp[1] * v[1], 		/**/ temp[2] * v[1] - s * v[0],
		temp[0] * v[2] - s * v[1],	/**/ temp[1] * v[2] + s * v[0],	/**/ c + temp[2] * v[2]
	);
}

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const Vec3<T>& axis, const T& angle) noexcept {
	return rotate(axis, angle) * m;
}

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, const Vec3<T>& axis, const T& angle) noexcept {
	const auto r = rotate(axis, angle);

	//Multiply the rotation matrix with the input matrix. Manually done to improve performance
	return Mat4x4<T>( 
		m[0]*r[0][0] + m[1]*r[0][1] + m[2]*r[0][2],
		m[0]*r[1][0] + m[1]*r[1][1] + m[2]*r[1][2],
		m[0]*r[2][0] + m[1]*r[2][1] + m[2]*r[2][2],
		m[3]
	);
}

template<typename T>
constexpr Mat3x3<T> rotate(const Quaternion<T>& quat) noexcept {
	return toMatrix(quat);
}

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const Quaternion<T>& quat) noexcept {
	return rotate(quat) * m;
}

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, const Quaternion<T>& quat) noexcept {
	const auto r = rotate(quat);

	//Multiply the rotation matrix with the input matrix. Manually done to improve performance
	return Mat4x4<T>( 
		m[0]*r[0][0] + m[1]*r[0][1] + m[2]*r[0][2],
		m[0]*r[1][0] + m[1]*r[1][1] + m[2]*r[1][2],
		m[0]*r[2][0] + m[1]*r[2][1] + m[2]*r[2][2],
		m[3]
	);
}

}