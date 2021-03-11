#pragma once

#include "Vector.h"
#include "BezierLoop.h"
#include "../Macros.h"

#include <vector>
#include <deque>
#include <utility>
#include <type_traits>

namespace Zuazo::Math {

enum class TriangleSideFlags {
	NONE		= 0,
	LEFT		= 1 << 0,
	RIGHT		= 1 << 1,
	BOTH		= LEFT | RIGHT
};

ZUAZO_ENUM_BIT_OPERATORS(TriangleSideFlags)

struct TriangleStripTag {};
constexpr TriangleStripTag triangleStrip;

struct TriangleListTag {};
constexpr TriangleStripTag triangleList;


template<typename T, typename I = uint32_t>
class Triangulator {
public:
	static_assert(std::is_integral<I>::value && std::is_unsigned<I>::value , "Index must be a unsigned integer");

	using value_type = T;
	using index_type = I;
	using polygon_type = Polygon<value_type>;
	using vector_type = typename polygon_type::value_type;

	using Diagonal = std::pair<index_type, index_type>;

	class TriangleListGenerator {
	public:
		constexpr TriangleListGenerator(std::vector<index_type>& result,
										index_type startIndex = index_type(0) ) noexcept;
		TriangleListGenerator(const TriangleListGenerator& other) = default;
		~TriangleListGenerator() = default;

		TriangleListGenerator&							operator=(const TriangleListGenerator& other) = default;

		TriangleSideFlags								operator()(	const Diagonal& diagonal, 
																	index_type bestVertex,
																	TriangleSideFlags adjointDiagonals,
																	size_t queueLength,
																	bool falseDiag );

	private:
		std::reference_wrapper<std::vector<index_type>>	m_result;
		index_type 										m_startIndex;

	};

	class TriangleStripGenerator {
	public:
		constexpr TriangleStripGenerator(	std::vector<index_type>& result,
											index_type startIndex = index_type(0),
											index_type restartIndex = ~index_type(0) ) noexcept;
		TriangleStripGenerator(const TriangleStripGenerator& other) = default;
		~TriangleStripGenerator() = default;

		TriangleStripGenerator&							operator=(const TriangleStripGenerator& other) = default;

		TriangleSideFlags								operator()(	const Diagonal& diagonal, 
																	index_type bestVertex,
																	TriangleSideFlags adjointDiagonals,
																	size_t queueLength,
																	bool falseDiag );

	private:
		std::reference_wrapper<std::vector<index_type>>	m_result;
		index_type 										m_startIndex;
		index_type 										m_restartIndex;
		TriangleSideFlags								m_stripingSide;
	};


	Triangulator() = default;
	Triangulator(const Triangulator& other) = delete;
	Triangulator(Triangulator&& other) = default;
	~Triangulator() = default;

	Triangulator&					operator=(const Triangulator& other) = delete;
	Triangulator&					operator=(Triangulator&& other) = default;

	template<typename F>
	void 							operator()(	const polygon_type& polygon,
												F&& triangleCallback ) const;

	static constexpr std::array<index_type, 6> 	triangulateQuad(TriangleListTag triangleListTag,
																const vector_type& v0,
																const vector_type& v1,
																const vector_type& v2,
																const vector_type& v3,
																index_type startIndex = index_type(0) ) noexcept;

	static constexpr std::array<index_type, 4> 	triangulateQuad(TriangleStripTag triangleStripTag,
																const vector_type& v0,
																const vector_type& v1,
																const vector_type& v2,
																const vector_type& v3,
																index_type startIndex = index_type(0) ) noexcept;

private:
	struct Visibility {
		constexpr Visibility(	bool visible = true, 
								value_type weight = 0, 
								index_type bestVertex = ~index_type() ) noexcept;

		bool 		visible;
		value_type 	weight;
		index_type	bestVertex;
	};

	mutable std::vector<Visibility>	m_visibilities;
	mutable std::deque<Diagonal>	m_diagonals;

};

}

#include "Triangulator.inl"