pragma once

#include "Vector.h"

#include <glm/detail/qualifier.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include <sys/types.h>

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

typedef Mat2x2<float> Mat2x2f;
typedef Mat2x3<float> Mat2x3f;
typedef Mat2x4<float> Mat2x4f;
typedef Mat3x2<float> Mat3x2f;
typedef Mat3x3<float> Mat3x3f;
typedef Mat3x4<float> Mat3x4f;
typedef Mat4x2<float> Mat4x2f;
typedef Mat4x3<float> Mat4x3f;
typedef Mat4x4<float> Mat4x4f;

typedef Mat2x2<int> Mat2x2i;
typedef Mat2x3<int> Mat2x3i;
typedef Mat2x4<int> Mat2x4i;
typedef Mat3x2<int> Mat3x2i;
typedef Mat3x3<int> Mat3x3i;
typedef Mat3x4<int> Mat3x4i;
typedef Mat4x2<int> Mat4x2i;
typedef Mat4x3<int> Mat4x3i;
typedef Mat4x4<int> Mat4x4i;

typedef Mat2x2<double> Mat2x2d;
typedef Mat2x3<double> Mat2x3d;
typedef Mat2x4<double> Mat2x4d;
typedef Mat3x2<double> Mat3x2d;
typedef Mat3x3<double> Mat3x3d;
typedef Mat3x4<double> Mat3x4d;
typedef Mat4x2<double> Mat4x2d;
typedef Mat4x3<double> Mat4x3d;
typedef Mat4x4<double> Mat4x4d;

}