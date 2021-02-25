#pragma once

#include "Vector.h"
#include "BezierLoop.h"

#include <vector>
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

private:
	mutable std::vector<index_type>	m_indices;
	mutable std::vector<index_type>	m_monotonePolygonIndices;
	mutable std::vector<index_type>	m_sortedIndices;

};

}

#include "Triangulator.inl"