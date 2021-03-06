#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Bezier.h"
#include "BezierLoop.h"

namespace Zuazo::Math {

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(const Vec2<T> vec) noexcept;

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(	const Vec2<T> vec,
																	normalized_t ) noexcept;

template<typename T, size_t N>
constexpr typename Vec<T, N>::value_type distance(const Vec<T, N>& a, const Vec<T, N>& b) noexcept;

template<typename T, size_t N>
constexpr typename Vec<T, N>::value_type distance2(const Vec<T, N>& a, const Vec<T, N>& b) noexcept;

template<typename T, size_t N>
constexpr typename Line<T, N>::value_type proj(const Line<T, N>& line, const typename Line<T, N>::value_type& p) noexcept;

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Vec2<T>& dir,
																		const Vec2<T>& p,
																		normalized_t ) noexcept;

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Vec2<T>& dir,
																		const Vec2<T>& p ) noexcept;

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Line<T, 2>& line,
																		const typename Line<T, 2>::value_type& point ) noexcept;

template<typename T>
constexpr typename Polygon<T>::value_type::value_type getSignedArea(const Polygon<T>& polygon) noexcept;

template<typename T>
constexpr Vec2<T> getBarycentricCoordinates(const Vec2<T>& t0,
											const Vec2<T>& t1,
											const Vec2<T>& t2,
											const Vec2<T>& p ) noexcept;

template<typename T>
constexpr Vec2<T> getBarycentricCoordinates(const Vec2<T>& t1,
											const Vec2<T>& t2,
											const Vec2<T>& p ) noexcept;

template<typename T>
constexpr typename Vec2<T>::value_type getSignedArea(	const Vec2<T>& v0,
														const Vec2<T>& v1,
														const Vec2<T>& v2 ) noexcept;

template<typename T>
constexpr typename Vec2<T>::value_type getWinding(	const Vec2<T>& v0,
													const Vec2<T>& v1,
													const Vec2<T>& v2 ) noexcept;


template<typename T>
constexpr bool isConvex(const Polygon<T>& polygon) noexcept;

template<typename T>
constexpr bool isConvex(const Vec2<T>& v0,
						const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& v3 ) noexcept;

template<typename T>
constexpr bool isInCone(const Vec2<T>& v0,
						const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& p ) noexcept;

template<typename T>
constexpr bool isInCone(const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& p ) noexcept;

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