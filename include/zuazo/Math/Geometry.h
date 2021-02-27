#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Bezier.h"

namespace Zuazo::Math {

struct normalized_t {};
constexpr normalized_t normalized;

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(const Vec2<T> vec) noexcept;

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(	const Vec2<T> vec,
																	normalized_t ) noexcept;

template<typename T, size_t N>
constexpr typename Vec<T, N>::value_type distance(const Vec<T, N>& a, const Vec<T, N>& b) noexcept;

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Line<T, 2>& line,
																		const typename Line<T, 2>::value_type& point ) noexcept;

template<typename T>
constexpr typename Polygon<T>::value_type::value_type getSignedArea(const Polygon<T>& polygon) noexcept;

template<typename T>
constexpr bool isConvex(const Polygon<T>& polygon) noexcept;

template<typename T>
constexpr bool isInsideTriangle(const Vec2<T>& t0,
								const Vec2<T>& t1,
								const Vec2<T>& t2,
								const Vec2<T>& v ) noexcept;

template<typename T>
constexpr bool isInsideTriangle(const Vec2<T>& t1,
								const Vec2<T>& t2,
								const Vec2<T>& v ) noexcept;

template<typename T>
constexpr bool getIntersection(	const Line<T, 2>& a,
								const Line<T, 2>& b,
								typename Line<T, 2>::value_type* point = nullptr ) noexcept;

}

#include "Geometry.inl"