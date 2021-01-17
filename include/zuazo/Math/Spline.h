#pragma once

#include "../Utils/BufferView.h"

#include <array>
#include <vector>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class Spline {
public:
	static_assert(Deg > 0, "Degree must be 1 or greater");
	using value_type = T;

	template<typename... Values>
	constexpr explicit Spline(Values&&... values);
	constexpr Spline(const Spline& other) = default;
	constexpr Spline(Spline&& other) = default;
	template<typename Q>
	constexpr Spline(const Spline<Q, Deg>& other);	
	~Spline() = default;

	constexpr Spline&					operator=(const Spline& other) = default;
	constexpr Spline&					operator=(Spline&& other) = default;
	template<typename Q>
	constexpr Spline&					operator=(const Spline<Q, Deg>& other);	

	constexpr value_type&				operator[](size_t i) noexcept;
	constexpr const value_type&			operator[](size_t i) const noexcept;

	template<typename Q>
	constexpr value_type 				sample(Q t) const;

	constexpr value_type&				front() noexcept;
	constexpr const value_type&			front() const noexcept;
	constexpr value_type&				back() noexcept;
	constexpr const value_type&			back() const noexcept;

	constexpr value_type*				begin() noexcept;
	constexpr const value_type*			begin() const noexcept;
	constexpr const value_type*			cbegin() const noexcept;
	constexpr value_type*				end() noexcept;
	constexpr const value_type*			end() const noexcept;
	constexpr const value_type*			cend() const noexcept;

	constexpr value_type*				data() noexcept;
	constexpr const value_type*			data() const noexcept;

	static constexpr size_t				size() noexcept;
	static constexpr size_t				degree() noexcept;

private:
	std::array<value_type, Deg+1>		m_values;

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
	using value_type = typename Spline::value_type;

	SplineLoop(Utils::BufferView<const value_type> values);
	SplineLoop(Utils::BufferView<const Spline> splines);
	SplineLoop(const SplineLoop& other) = default;
	SplineLoop(SplineLoop&& other) = default;
	~SplineLoop() = default;

	SplineLoop&									operator=(const SplineLoop& other) = default;
	SplineLoop&									operator=(SplineLoop&& other) = default;

	void 										setSpline(size_t i, const Spline& s);
	const Spline&								getSpline(size_t i) const noexcept;

	template<typename Q>
	value_type 									sample(Q t) const;

	const value_type*							data();
	size_t										size() const;
	size_t										splineCount() const;

	static constexpr size_t						degree();

private:
	std::vector<std::array<value_type, Deg>>	m_values;

};

template<typename T>
using LinearBezierLoop = SplineLoop<T, 1>;

template<typename T>
using QuadraticBezierLoop = SplineLoop<T, 2>;

template<typename T>
using CubicBezierLoop = SplineLoop<T, 3>;

}

#include "Spline.inl"