#pragma once

#include "Vector.h"
#include "Matrix.h"

#include <glm/gtc/quaternion.hpp>

namespace Zuazo::Math {

template <typename T>
using Quaternion = glm::qua<T, glm::qualifier::defaultp>;

using Quaternionf = Quaternion<glm::f32>;
using Quaterniond = Quaternion<glm::f64>;


template<typename T>
Mat4x4<T> toMatrix(const Quaternion<T>& q) noexcept;

template<typename T>
Quaternion<T> fromEulerAngles(const Vec3<T>& angles) noexcept;

template<typename T>
Quaternion<T> fromEulerAngles(const T& yaw, const T& pitch, const T& roll) noexcept;

template<typename T>
Vec3<T> toEulerAngles(const Quaternion<T>& q) noexcept;

template<typename T>
T getYaw(const Quaternion<T>& q) noexcept;

template<typename T>
T getPitch(const Quaternion<T>& q) noexcept;

template<typename T>
T getRoll(const Quaternion<T>& q) noexcept;

}

#include "Quaternion.inl"