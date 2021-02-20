#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Bezier.h"

namespace Zuazo::Math {

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(const Vec2<T> vec) noexcept;

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Line<T, 2>& line,
																		const typename Line<T, 2>::value_type& point ) noexcept;


template<typename T, typename Index = uint32_t>
std::vector<Index> triangulate(Utils::BufferView<const Vec2<T>> polygon);

}

#include "Geometry.inl"