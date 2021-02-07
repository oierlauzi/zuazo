#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Zuazo::Math {

template<typename T>
constexpr Mat4x4<T> frustum(T left, 		T right, 
							T bottom, 	T top, 
							T near, 	T far ) noexcept;

template<typename T>
constexpr Mat4x4<T> perspective(T fovy, 
								T aspect, 
								T near ) noexcept;

template<typename T>
constexpr Mat4x4<T> perspective(T fovy, 
								T aspect, 
								T near,
								T far ) noexcept;

template<typename T>
constexpr Mat4x4<T> ortho(	T left, 	T right, 
							T bottom, 	T top  ) noexcept;

template<typename T>
constexpr Mat4x4<T> ortho(	T left, 	T right, 
							T bottom, 	T top, 
							T near, 	T far ) noexcept;



template<typename T>
constexpr Mat4x4<T> lookAt(	const Vec3<T>& eye,
							const Vec3<T>& center,
							const Vec3<T>& up ) noexcept;



template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> translate(const Vec<T, N>& delta) noexcept;

template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> translate(const Mat<T, N+1, N+1>& m, const Vec<T, N>& delta) noexcept;


template<typename T, size_t N>
constexpr Mat<T, N, N> scale(const Vec<T, N>& s) noexcept;

template<typename T, size_t N>
constexpr Mat<T, N, N> scale(const Mat<T, N, N>& m, const Vec<T, N>& s) noexcept;

template<typename T, size_t N>
constexpr Mat<T, N+1, N+1> scale(const Mat<T, N+1, N+1>& m, const Vec<T, N>& s) noexcept;


template<typename T>
constexpr Mat2x2<T> rotate(const T& angle) noexcept;

template<typename T>
constexpr Mat2x2<T> rotate(const Mat2x2<T>& m, const T& angle) noexcept;

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const T& angle) noexcept;

template<typename T>
constexpr Mat3x3<T> rotate(const Vec3<T>& axis, const T& angle) noexcept;

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const Vec3<T>& axis, const T& angle) noexcept;

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, const Vec3<T>& axis, const T& angle) noexcept;

template<typename T>
constexpr Mat3x3<T> rotate(const Quaternion<T>& quat) noexcept;

template<typename T>
constexpr Mat3x3<T> rotate(const Mat3x3<T>& m, const Quaternion<T>& quat) noexcept;

template<typename T>
constexpr Mat4x4<T> rotate(const Mat4x4<T>& m, const Quaternion<T>& quat) noexcept;

}

#include "Transformations.inl"