#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Zuazo::Math {

template <typename T, int dim>
using Vec = glm::vec<dim, T>;

template <typename T>
using Vec2 = Vec<T, 2>;

template <typename T>
using Vec3 = Vec<T, 3>;

template <typename T>
using Vec4 = Vec<T, 4>;

using Vec2f = Vec<glm::f32, 2>;
using Vec3f = Vec<glm::f32, 3>;
using Vec4f = Vec<glm::f32, 4>;

using Vec2d = Vec<glm::f64, 2>;
using Vec3d = Vec<glm::f64, 3>;
using Vec4d = Vec<glm::f64, 4>;

using Vec2i = Vec<glm::i32, 2>;
using Vec3i = Vec<glm::i32, 3>;
using Vec4i = Vec<glm::i32, 4>;

using Vec2u = Vec<glm::u32, 2>;
using Vec3u = Vec<glm::u32, 3>;
using Vec4u = Vec<glm::u32, 4>;


}