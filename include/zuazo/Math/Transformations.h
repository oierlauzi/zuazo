#pragma once

#include "Vector.h"
#include "Matrix.h"

namespace Zuazo::Math {

template<typename T>
Mat4x4<T> frustum(	T left, 	T right, 
					T bottom, 	T top, 
					T near, 	T far );

template<typename T>
Mat4x4<T> perspective(	T fovy, 
						T aspect, 
						T near );

template<typename T>
Mat4x4<T> perspective(	T fovy, 
						T aspect, 
						T near,
						T far );

template<typename T>
Mat4x4<T> ortho(T left, 	T right, 
				T bottom, 	T top  );

template<typename T>
Mat4x4<T> ortho(T left, 	T right, 
				T bottom, 	T top, 
				T near, 	T far );



template<typename T>
Mat4x4<T> lookAt(	const Vec3<T>& eye,
					const Vec3<T>& center,
					const Vec3<T>& up );

template<typename T>
Mat4x4<T> translate(const Mat4x4<T>& mat, const Vec3<T>& delta);

template<typename T>
Mat4x4<T> scale(const Mat4x4<T>& mat, const Vec3<T>& scale);

template<typename T>
Mat4x4<T> rotate(const Mat4x4<T>& mat, T angle, const Vec3<T>& axis);

}

#include "Transformations.inl"