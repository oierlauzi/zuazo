#pragma once

#define GLM_HAS_ONLY_XYZW

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

namespace Zuazo::Utils{

template <typename T>
using Vec2 = glm::vec<2, T, glm::qualifier::defaultp>;

template <typename T>
using Vec3 = glm::vec<3, T, glm::qualifier::defaultp>;

template <typename T>
using Vec4 = glm::vec<4, T, glm::qualifier::defaultp>;

}
