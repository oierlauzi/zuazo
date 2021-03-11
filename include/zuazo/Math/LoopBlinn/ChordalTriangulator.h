#pragma once

#include "Vertex.h"
#include "../Triangulator.h"
#include "../../Utils/BufferView.h"

#include <type_traits>
#include <utility>
#include <cstdint>

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I = uint32_t>
class ChordalTriangulator {
public:
	using value_type = T;
	using index_type = I;
	using triangulator_type = Triangulator<T, I>;
	using vertex_type = Vertex<T>;
	using position_vector_type = typename vertex_type::position_vector_type;
	using klm_vector_type = typename vertex_type::klm_vector_type;

	struct VertexReference {
		constexpr VertexReference(	index_type front,
									index_type back,
									index_type helper ) noexcept;

		index_type front;
		index_type back;
		index_type helper;
	};

	static_assert(std::is_same<position_vector_type, typename triangulator_type::vector_type>::value, "Vector type missmatch");

	ChordalTriangulator(std::vector<index_type>& indices,
						std::vector<vertex_type>& chordalAxis,
						Utils::BufferView<const vertex_type> vertices,
						Utils::BufferView<const VertexReference> references,
						index_type chordalBaseIndex,
						index_type restartIndex = ~index_type(0) );
	ChordalTriangulator(const ChordalTriangulator& other) = default;
	~ChordalTriangulator() = default;

	ChordalTriangulator&							operator=(const ChordalTriangulator& other) = default;

	TriangleSideFlags								operator()(	const typename triangulator_type::Diagonal& diagonal, 
																typename triangulator_type::index_type bestVertex,
																TriangleSideFlags adjointDiagonals,
																size_t queueLength,
																bool falseDiag );

private:
	std::reference_wrapper<std::vector<index_type>>	m_indices;
	std::reference_wrapper<std::vector<vertex_type>>m_chordalAxis;
	Utils::BufferView<const vertex_type>			m_vertices;
	Utils::BufferView<const VertexReference>		m_references;
	index_type										m_chordalBaseIndex;
	index_type										m_restartIndex;

	vertex_type										getChordalVertex(	index_type index,
																		const position_vector_type& pos ) const noexcept;

	static constexpr klm_vector_type				extrapolate(const vertex_type& v0,	
																const vertex_type& v1,
																const vertex_type& v2,
																const position_vector_type& pos ) noexcept;

};

}

#include "ChordalTriangulator.inl"