#include "Transformations.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Zuazo::Math {

template<typename T>
inline Mat4x4<T> frustum(	T left, 	T right, 
							T bottom, 	T top, 
							T near, 	T far )
{
	return glm::frustum(
		left, right, 
		bottom, top, 
		near, far
	);
}

template<typename T>
inline Mat4x4<T> perspective(	T fovy, 
								T aspect, 
								T near )
{
	return glm::infinitePerspective(fovy, aspect, near);
}

template<typename T>
inline Mat4x4<T> perspective(	T fovy, 
								T aspect, 
								T near,
								T far )
{
	return glm::perspective(fovy, aspect, near, far);
}

template<typename T>
inline Mat4x4<T> ortho(	T left, 	T right, 
						T bottom, 	T top  )
{
	return glm::ortho(
		left, right, 
		bottom, top
	);
}

template<typename T>
inline Mat4x4<T> ortho(	T left, 	T right, 
						T bottom, 	T top, 
						T near, 	T far )
{
	return glm::ortho(
		left, right, 
		bottom, top, 
		near, far
	);
}



template<typename T>
inline Mat4x4<T> lookAt(const Vec3<T>& eye,
						const Vec3<T>& center,
						const Vec3<T>& up )
{
	glm::lookAt(eye, center, up);
}

template<typename T>
inline Mat4x4<T> translate(const Mat4x4<T>& mat, const Vec3<T>& delta) {
	glm::translate(mat, delta);
}

template<typename T>
inline Mat4x4<T> scale(const Mat4x4<T>& mat, const Vec3<T>& scale) {
	return glm::scale(mat, scale);
}

template<typename T>
inline Mat4x4<T> rotate(const Mat4x4<T>& mat, const Quaternion<T>& quat) {
	return toMatrix(quat) * mat;
}

template<typename T>
inline Mat4x4<T> rotate(const Mat4x4<T>& mat, T angle, const Vec3<T>& axis) {
	return glm::rotate(mat, angle, axis);
}

}