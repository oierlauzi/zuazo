#pragma once

#include "Vector.h"

namespace Zuazo::Math {

template <typename T, int N, int M>
using Mat = glm::mat<N, M, T, glm::qualifier::defaultp>;

template <typename T>
using Mat2x2 = Mat<T, 2, 2>;
template <typename T>
using Mat2x3 = Mat<T, 2, 3>;
template <typename T>
using Mat2x4 = Mat<T, 2, 4>;

template <typename T>
using Mat3x2 = Mat<T, 3, 2>;
template <typename T>
using Mat3x3 = Mat<T, 3, 3>;
template <typename T>
using Mat3x4 = Mat<T, 3, 4>;

template <typename T>
using Mat4x2 = Mat<T, 4, 2>;
template <typename T>
using Mat4x3 = Mat<T, 4, 3>;
template <typename T>
using Mat4x4 = Mat<T, 4, 4>;

typedef Mat2x2<glm::f32> Mat2x2f;
typedef Mat2x3<glm::f32> Mat2x3f;
typedef Mat2x4<glm::f32> Mat2x4f;
typedef Mat3x2<glm::f32> Mat3x2f;
typedef Mat3x3<glm::f32> Mat3x3f;
typedef Mat3x4<glm::f32> Mat3x4f;
typedef Mat4x2<glm::f32> Mat4x2f;
typedef Mat4x3<glm::f32> Mat4x3f;
typedef Mat4x4<glm::f32> Mat4x4f;

typedef Mat2x2<glm::i32> Mat2x2i;
typedef Mat2x3<glm::i32> Mat2x3i;
typedef Mat2x4<glm::i32> Mat2x4i;
typedef Mat3x2<glm::i32> Mat3x2i;
typedef Mat3x3<glm::i32> Mat3x3i;
typedef Mat3x4<glm::i32> Mat3x4i;
typedef Mat4x2<glm::i32> Mat4x2i;
typedef Mat4x3<glm::i32> Mat4x3i;
typedef Mat4x4<glm::i32> Mat4x4i;

typedef Mat2x2<glm::f64> Mat2x2d;
typedef Mat2x3<glm::f64> Mat2x3d;
typedef Mat2x4<glm::f64> Mat2x4d;
typedef Mat3x2<glm::f64> Mat3x2d;
typedef Mat3x3<glm::f64> Mat3x3d;
typedef Mat3x4<glm::f64> Mat3x4d;
typedef Mat4x2<glm::f64> Mat4x2d;
typedef Mat4x3<glm::f64> Mat4x3d;
typedef Mat4x4<glm::f64> Mat4x4d;

template <typename T, int N, int M>
constexpr Mat<T, M, N> transpose(const Mat<T, N, M>& m);

template <typename T, int N>
constexpr Mat<T, N, N> inv(const Mat<T, N, N>& m);

template <typename T>
constexpr Mat<T, 2, 2> diagonal(const Vec<T, 2>& v);

template <typename T>
constexpr Mat<T, 3, 3> diagonal(const Vec<T, 3>& v);

template <typename T>
constexpr Mat<T, 4, 4> diagonal(const Vec<T, 4>& v);

}

#include "Matrix.inl"