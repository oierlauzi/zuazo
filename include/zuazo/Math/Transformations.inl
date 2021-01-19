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
constexpr Mat<T, N+1, N+1> translate(Mat<T, N+1, N+1> m, const Vec<T, N>& delta) noexcept {
	for(size_t i = 0; i < delta.size(); ++i) {
		m[m.columns()-1] += m[i]*delta[i];
	}

	return m;
}

template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> scale(Mat<T, N+1, N+1> m, const Vec<T, N>& scale) noexcept {
	for(size_t i = 0; i < scale.size(); ++i) {
		m[i] *= scale[i];
	}

	return m;
}

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, const Quaternion<T>& quat) noexcept {
	return Mat4x4<T>(toMatrix(quat)) * m;
}

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, T angle) noexcept {
	Mat3x3<T> result;
	const auto c = cos(a);
	const auto s = sin(a);

	Mat2x2<T> rotate;
	rotate[0][0] = c;
	rotate[0][1] = s;
	rotate[1][0] = -s;
	rotate[1][1] = c; 

	//Multiply the rotation matrix with the input matrix. Manually done to improve performance
	result[0] = m[0]*rotate[0][0] + m[1]*rotate[0][1];
	result[1] = m[0]*rotate[1][0] + m[1]*rotate[1][1];
	result[2] = m[2];

	return result;
}

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, T angle, const Vec3<T>& v) noexcept {
	Mat4x4<T> result;
	const auto a = angle;
	const auto c = cos(a);
	const auto s = sin(a);
	const auto axis = normalize(v);
	const auto temp = (static_cast<T>(1) - c) * axis;

	Mat3x3<T> rotate;
	rotate[0][0] = c + temp[0] * axis[0];
	rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	rotate[0][2] = temp[0] * axis[2] - s * axis[1];
	rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	rotate[1][1] = c + temp[1] * axis[1];
	rotate[1][2] = temp[1] * axis[2] + s * axis[0];
	rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	rotate[2][2] = c + temp[2] * axis[2];

	//Multiply the rotation matrix with the input matrix. Manually done to improve performance
	result[0] = m[0]*rotate[0][0] + m[1]*rotate[0][1] + m[2]*rotate[0][2];
	result[1] = m[0]*rotate[1][0] + m[1]*rotate[1][1] + m[2]*rotate[1][2];
	result[2] = m[0]*rotate[2][0] + m[1]*rotate[2][1] + m[2]*rotate[2][2];
	result[3] = m[3];

	return result;
}

}