#pragma once

#define GLM_HAS_ONLY_XYZW

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

namespace Zuazo{

template <typename T>
using Vec2 = glm::tvec2<T>;

template <typename T>
using Vec3 = glm::tvec3<T>;

template <typename T>
using Vec4 = glm::tvec4<T>;

}
