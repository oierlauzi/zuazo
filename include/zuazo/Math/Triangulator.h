#pragma once

#include "Vector.h"
#include "BezierLoop.h"

#include <vector>
#include <deque>
#include <utility>
#include <type_traits>

namespace Zuazo::Math {

template<typename T, typename I = uint32_t>
class Triangulator {
public:
	static_assert(std::is_integral<I>::value && std::is_unsigned<I>::value , "Index must be a unsigned integer");

	using value_type = T;
	using index_type = I;
	using polygon_type = Polygon<value_type>;
	using vector_type = typename polygon_type::value_type;

	Triangulator() = default;
	Triangulator(const Triangulator& other) = delete;
	Triangulator(Triangulator&& other) = default;
	~Triangulator() = default;

	Triangulator&					operator=(const Triangulator& other) = delete;
	Triangulator&					operator=(Triangulator&& other) = default;

	void							operator()(	std::vector<index_type>& result,
												const polygon_type& polygon,
												index_type startIndex = index_type(0),
												index_type restartIndex = ~index_type(0) ) const;

	std::vector<index_type> 		operator()(	const polygon_type& polygon,
												index_type startIndex = index_type(0),
												index_type restartIndex = ~index_type(0) ) const;

	static constexpr std::array<index_type, 4> 	triangulateQuad(const vector_type& v0,
																const vector_type& v1,
																const vector_type& v2,
																const vector_type& v3,
																index_type startIndex = index_type(0) ) noexcept;

private:
	using Diagonal = std::pair<index_type, index_type>;

	struct DPState {
		constexpr DPState(	bool visible = true, 
							value_type weight = 0, 
							index_type bestVertex = ~index_type() ) noexcept;

		bool 		visible;
		value_type 	weight;
		index_type	bestVertex;
	};

	mutable std::vector<DPState>	m_dpState;
	mutable std::deque<Diagonal>	m_diagonals;


	static constexpr bool			isBelow(const vector_type& delta) noexcept;
	static constexpr bool			isAbove(const vector_type& delta) noexcept;


};

}

#include "Triangulator.inl"