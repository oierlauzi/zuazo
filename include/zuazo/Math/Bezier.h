#pragma once

#include "Polynomial.h"
#include "../Utils/BufferView.h"
#include "../Utils/Limit.h"

#include <array>
#include <vector>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class Bezier {
public:
	using value_type = T;

	template<typename... Values>
	constexpr explicit Bezier(Values&&... values);
	constexpr Bezier(const Bezier& other) = default;
	constexpr Bezier(Bezier&& other) = default;
	template<typename Q>
	constexpr Bezier(const Bezier<Q, Deg>& other);	
	~Bezier() = default;

	constexpr Bezier&						operator=(const Bezier& other) = default;
	constexpr Bezier&						operator=(Bezier&& other) = default;
	template<typename Q>
	constexpr Bezier&						operator=(const Bezier<Q, Deg>& other);	

	constexpr value_type&					operator[](size_t i) noexcept;
	constexpr const value_type&				operator[](size_t i) const noexcept;

	template<typename Q>
	constexpr value_type 					operator()(Q t) const;

	constexpr value_type&					front() noexcept;
	constexpr const value_type&				front() const noexcept;
	constexpr value_type&					back() noexcept;
	constexpr const value_type&				back() const noexcept;

	constexpr value_type*					begin() noexcept;
	constexpr const value_type*				begin() const noexcept;
	constexpr const value_type*				cbegin() const noexcept;
	constexpr value_type*					end() noexcept;
	constexpr const value_type*				end() const noexcept;
	constexpr const value_type*				cend() const noexcept;

	constexpr value_type*					data() noexcept;
	constexpr const value_type*				data() const noexcept;

	static constexpr size_t					size() noexcept;
	static constexpr size_t					degree() noexcept;

private:
	std::array<value_type, Deg+1>			m_values;

};

template<typename T>
using LinearBezier = Bezier<T, 1>;

template<typename T>
using QuadraticBezier = Bezier<T, 2>;

template<typename T>
using CubicBezier = Bezier<T, 3>;


template<typename T, size_t Deg>
constexpr Polynomial<typename Bezier<T, Deg>::value_type, Bezier<T, Deg>::degree()> toPolynomial(const Bezier<T, Deg>& s);

template<typename T, size_t Deg>
constexpr Bezier<T, Deg-1> derivate(const Bezier<T, Deg>& s);

template<typename T, size_t Deg>
constexpr std::array<typename Bezier<T, Deg>::value_type, Bezier<T, Deg>::degree()> solve(	const Bezier<T, Deg>& s, 
																							PolynomialSolutionCount* cnt);

template<typename T, size_t N, size_t Deg>
constexpr std::array<typename Bezier<Vec<T, N>, Deg>::value_type, Bezier<Vec<T, N>, Deg>::degree()> solve(	const Bezier<Vec<T, N>, Deg>& s, 
																											Vec<PolynomialSolutionCount, N>* cnt );

template<typename T, size_t Deg>
constexpr Utils::Range<typename Bezier<T, Deg>::value_type> getBoundaries(const Bezier<T, Deg>& s);

template<typename T, size_t N, size_t Deg>
constexpr Utils::Range<typename Bezier<Vec<T, N>, Deg>::value_type> getBoundaries(const Bezier<Vec<T, N>, Deg>& s);


template<typename T, size_t Deg>
class BezierLoop {
public:
	static_assert(Deg > 0, "Degree must be 1 or greater");
	
	using Bezier = Math::Bezier<T, Deg>;
	using value_type = typename Bezier::value_type;

	BezierLoop(Utils::BufferView<const value_type> values);
	BezierLoop(Utils::BufferView<const Bezier> splines);
	BezierLoop(const BezierLoop& other) = default;
	BezierLoop(BezierLoop&& other) = default;
	~BezierLoop() = default;

	BezierLoop&									operator=(const BezierLoop& other) = default;
	BezierLoop&									operator=(BezierLoop&& other) = default;

	void 										setBezier(size_t i, const Bezier& s);
	const Bezier&								getBezier(size_t i) const noexcept;

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
using LinearBezierLoop = BezierLoop<T, 1>;

template<typename T>
using QuadraticBezierLoop = BezierLoop<T, 2>;

template<typename T>
using CubicBezierLoop = BezierLoop<T, 3>;

}

#include "Bezier.inl"