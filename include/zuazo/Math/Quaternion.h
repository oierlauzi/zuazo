#pragma once

#include "Vector.h"
#include "Matrix.h"

#include <glm/gtc/quaternion.hpp>

namespace Zuazo::Math {

template <typename T>
using Quaternion = glm::qua<T, glm::qualifier::defaultp>;

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;


template<typename T>
Mat4x4<T> toMatrix(const Quaternion<T>& q);

template<typename T>
Vec3<T> getEulerAngles(const Quaternion<T>& q);

template<typename T>
T getYaw(const Quaternion<T>& q);

template<typename T>
T getPitch(const Quaternion<T>& q);

template<typename T>
T getRoll(const Quaternion<T>& q);


template<typename T>
Quaternion<T> lookAt(const Vec3<T>& direction, const Vec3<T>& up);

}

#include "Quaternion.inl"