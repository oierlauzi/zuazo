#pragma once

#include <array>
#include <vector>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class Spline {
public:
	using Point = T;

	Point 								sample(float t) const noexcept;

	static constexpr size_t				getDegree() noexcept;

private:
	std::array<Point, Deg+1>			m_points;
};

template<typename T>
using LinearBezier = Spline<T, 1>;

template<typename T>
using QuadraticBezier = Spline<T, 2>;

template<typename T>
using CubicBezier = Spline<T, 3>;



template<typename T, size_t Deg>
class SplineLoop {
public:
	using Spline = Math::Spline<T, Deg>;

	size_t								size() const noexcept;

private:
	std::vector<std::array<Point, Deg>>	m_points;

};

template<typename T>
using LinearBezierLoop = SplineLoop<T, 1>;

template<typename T>
using QuadraticBezierLoop = SplineLoop<T, 2>;

template<typename T>
using CubicBezierLoop = SplineLoop<T, 3>;

}

#include "Spline.inl"