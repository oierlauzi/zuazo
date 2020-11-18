#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Zuazo::Math {

template <typename T, int dim>
using Vec = glm::vec<dim, T, glm::qualifier::defaultp>;

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


/*
 * Absolute
 */

template<typename T, int dim>
Math::Vec<T, dim> abs(const Math::Vec<T, dim>& x) noexcept;


/*
 * Comparisons
 */

template<typename T, int dim>
constexpr Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
constexpr Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi) noexcept;

template<typename T, int dim>
constexpr bool isInRange(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi) noexcept;

/*
 * Exponential
 */

template<typename T, int dim>
Math::Vec<T, dim> exp(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> exp2(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> exp10(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> log(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> log2(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> log10(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> pow(const Math::Vec<T, dim>& base, const Math::Vec<T, dim>& power) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> sqrt(const Math::Vec<T, dim>& x) noexcept;


/*
 * Factor
 */

template<typename T, int dim>
Math::Vec<T, dim> mod(const Math::Vec<T, dim>& num, const Math::Vec<T, dim>& den) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> gcd(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;

template<typename T, int dim>
Math::Vec<T, dim> lcm(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) noexcept;


/*
 * Rounding
 */

template<typename T, int dim, typename Q>
Math::Vec<T, dim> floor(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> trunc(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> ceil(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> round(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> fract(const Math::Vec<T, dim>& x) noexcept;


/*
 * Trigonometry
 */

template<typename T, int dim, typename Q>
Math::Vec<T, dim> cos(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> cosh(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> sin(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> sinh(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> tan(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> tan(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> tanh(const Math::Vec<T, dim>& x) noexcept;


template<typename T, int dim, typename Q>
Math::Vec<T, dim> acos(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> acosh(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> asin(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> asinh(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> atan(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> atan(const Math::Vec<T, dim>& y, const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> atan(const Math::Vec<T, dim>& x) noexcept;

template<typename T, int dim, typename Q>
Math::Vec<T, dim> atanh(const Math::Vec<T, dim>& x) noexcept;

}

#include "Vector.inl"