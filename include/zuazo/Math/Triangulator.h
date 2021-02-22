#pragma once

#include "Vector.h"
#include "../Utils/BufferView.h"

#include <vector>
#include <type_traits>

namespace Zuazo::Math {

template<typename T, typename Index = uint32_t>
class Triangulator {
public:
	static_assert(std::is_integral<Index>::value && std::is_unsigned<Index>::value , "Index must be a unsigned integer");

	using vector_type = Vec2<T>;
	using index_type = Index;
	using value_type = typename vector_type::value_type;

	Triangulator() = default;
	Triangulator(const Triangulator& other) = delete;
	Triangulator(Triangulator&& other) = default;
	~Triangulator();

	Triangulator&			operator=(const Triangulator& other) = delete;
	Triangulator&			operator=(Triangulator&& other) = default;

	std::vector<index_type> 		operator()(	Utils::BufferView<const vector_type> polygon,
												index_type restartIndex = ~index_type(0)) const;

private:
	mutable std::vector<index_type>	m_indices;
	mutable std::vector<index_type>	m_monotonePolygonIndices;
	mutable std::vector<index_type>	m_sortedIndices;

};

}

#include "Triangulator.inl"