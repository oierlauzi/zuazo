#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <cstddef>
#include <string>

namespace Zuazo::Math {

template <typename T, intmax_t dim>
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


template<typename T, int dim>
constexpr Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b);

template<typename T, int dim>
constexpr Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b);

template<typename T, int dim>
constexpr Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi);

template<typename T, int dim, typename Q>
constexpr Math::Vec<T, dim> lerp(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b, const Q& r);

}

namespace Zuazo {

template<typename T, intmax_t dim>
std::string	toString(const T& a);

}

#include "Vector.inl"