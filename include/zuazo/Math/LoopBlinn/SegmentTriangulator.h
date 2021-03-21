#pragma once

#include "Classifier.h"
#include "KLMCalculator.h"
#include "Vertex.h"
#include "../Bezier.h"
#include "../../Utils/BufferView.h"

#include <cstdint>
#include <array>
#include <utility>

namespace Zuazo::Math::LoopBlinn {

enum class FillSide {
	LEFT,
	RIGHT
};

enum class VertexType {
	UNKNOWN = -1,
	FIRST,
	LAST,
	CONTROL_PROTRUDING,
	CONTROL,
};

template<typename T, typename I = uint32_t>
class SegmentTriangulator {
public:
	using value_type = T;
	using index_type = I;
	using vertex_type = Vertex<value_type>;
	using position_vector_type = typename vertex_type::position_vector_type;
	using klm_vector_type = typename vertex_type::klm_vector_type;
	using bezier_type = CubicBezier<position_vector_type>;
	using classifier_type = Classifier<value_type>;
	using klm_calculator_type = KLMCalculator<value_type>;

	struct VertexData {
		constexpr VertexData(	const position_vector_type& position = position_vector_type(0),
								const klm_vector_type& klm = klm_vector_type(-1),
								VertexType type = VertexType::UNKNOWN,
								index_type helperIndex = ~index_type(0) ) noexcept;

		vertex_type 	vertex;
		VertexType		type;
		index_type		helperIndex;
	};

	class Result {
	public:
		Result(	Utils::BufferView<const VertexData> vertices = {}, 
				Utils::BufferView<const index_type> indices = {} ) noexcept;
		Result(const Result& other) = default;
		~Result() = default;

		Result&								operator=(const Result& other) = default;

		void								setVertexCount(size_t count) noexcept;
		size_t								getVertexCount() const noexcept;
		
		void								setIndexCount(size_t count) noexcept;
		size_t								getIndexCount() const noexcept;

		void								setVertices(Utils::BufferView<const VertexData> vert) noexcept;
		Utils::BufferView<VertexData>		getVertices() noexcept;
		Utils::BufferView<const VertexData>	getVertices() const noexcept;

		void								setIndices(Utils::BufferView<const index_type> ind) noexcept;
		Utils::BufferView<index_type>		getIndices() noexcept;
		Utils::BufferView<const index_type>	getIndices() const noexcept;

	private:
		std::array<VertexData, 8>			m_vertices;
		std::array<index_type, 16>			m_indices;
		size_t								m_vertexCount;
		size_t								m_indexCount;
	};

	SegmentTriangulator() = default;
	SegmentTriangulator(const SegmentTriangulator& other) = default;
	~SegmentTriangulator() = default;

	SegmentTriangulator&		operator=(const SegmentTriangulator& other) = default;

	Result						operator()(	const bezier_type& bezier, 
											FillSide fillSide,
											index_type baseIndex = index_type(0),
											index_type restartIndex = ~index_type(0) ) const noexcept;

	Result						operator()(	const bezier_type& bezier, 
											const typename classifier_type::Result& classification,
											FillSide fillSide,
											index_type baseIndex = index_type(0),
											index_type restartIndex = ~index_type(0) ) const noexcept;

	Result						operator()(	const bezier_type& bezier, 
											const typename klm_calculator_type::Result& klmCoords,
											FillSide fillSide,
											index_type baseIndex = index_type(0),
											index_type restartIndex = ~index_type(0) ) const noexcept;

private:

};

}

#include "SegmentTriangulator.inl"