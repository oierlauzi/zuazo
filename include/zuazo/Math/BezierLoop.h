#pragma once

#include "Bezier.h"

#include <vector>
#include <array>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class BezierLoop {
public:
	static_assert(Deg > 0, "Degree must be 1 or greater");
	
	using bezier_type = Math::Bezier<T, Deg>;
	using value_type = typename bezier_type::value_type;
	using segment_data = std::array<value_type, bezier_type::degree()>;

	BezierLoop() = default;
	BezierLoop(Utils::BufferView<const segment_data> data);
	BezierLoop(Utils::BufferView<const bezier_type> segments);
	BezierLoop(const BezierLoop& other) = default;
	BezierLoop(BezierLoop&& other) = default;
	~BezierLoop() = default;

	BezierLoop&									operator=(const BezierLoop& other) = default;
	BezierLoop&									operator=(BezierLoop&& other) = default;

	void										setSegmentCount(size_t count);
	size_t										getSegmentCount() const noexcept;

	void 										setSegment(size_t i, const bezier_type& s);
	const bezier_type&							getSegment(size_t i) const noexcept;

	void										setPoint(size_t i, const value_type& v);
	const value_type&							getPoint(size_t i) const noexcept;

	template<typename Q>
	value_type 									operator()(Q t) const;

	const value_type*							data() const noexcept;
	size_t										size() const noexcept;

	const value_type*							begin() const noexcept;
	const value_type*							cbegin() const noexcept;
	const value_type*							end() const noexcept;
	const value_type*							cend() const noexcept;

	void										clear() noexcept;
	void										reverse() noexcept;

	void										lineTo(const value_type& value);
	void										pushSegment(const bezier_type& bezier);

	static constexpr size_t						degree() noexcept;

private:
	std::vector<value_type>						m_values;

};

template<typename T>
using LinearBezierLoop = BezierLoop<T, 1>;

template<typename T>
using QuadraticBezierLoop = BezierLoop<T, 2>;

template<typename T>
using CubicBezierLoop = BezierLoop<T, 3>;

template<typename T>
using Polygon = LinearBezierLoop<Vec<T, 2>>;


template<typename T, size_t Deg>
bool operator==(const BezierLoop<T, Deg>& lhs, const BezierLoop<T, Deg>& rhs) noexcept;
template<typename T, size_t Deg>
bool operator!=(const BezierLoop<T, Deg>& lhs, const BezierLoop<T, Deg>& rhs) noexcept;


template<typename T, size_t Deg>
Utils::Range<typename BezierLoop<T, Deg>::value_type> getBoundaries(const BezierLoop<T, Deg>& s);

}

#include "BezierLoop.inl"