#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <cstddef>
#include <ostream>

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
constexpr Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi) noexcept;

template<typename T, int dim, typename Q>
constexpr Math::Vec<T, dim> lerp(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b, const Q& r) noexcept;

template<typename T, int dim, typename Q = double>
constexpr Math::Vec<Q, dim> ilerp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim, typename Q = double>
constexpr Math::Vec<T, dim> map(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo1, const Math::Vec<T, dim>& hi1, const Math::Vec<T, dim>& lo2, const Math::Vec<T, dim>& hi2) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> mod(const Math::Vec<T, dim>& num, const Math::Vec<T, dim>& den) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> abs(const Math::Vec<T, dim>& r) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> inv(const Math::Vec<T, dim>& r) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> floor(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> trunc(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> ceil(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> round(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> frac(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> gcd(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> lcm(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

}

#include "Vector.inl"