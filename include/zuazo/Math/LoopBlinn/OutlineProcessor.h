#pragma once

#include "Classifier.h"
#include "KLMCalculator.h"
#include "Vertex.h"
#include "SegmentTriangulator.h"
#include "ChordalTriangulator.h"
#include "../Vector.h"
#include "../BezierLoop.h"
#include "../Triangulator.h"
#include "../../Utils/BufferView.h"

#include <utility>

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I = uint32_t>
class OutlineProcessor {
public:
	using value_type = T;
	using index_type = I;
	using polygon_triangulator_type = Triangulator<value_type, index_type>;
	using polygon_type = typename polygon_triangulator_type::polygon_type;
	using chordal_triangulator_type = ChordalTriangulator<value_type, index_type>;
	using vertex_reference_type = typename chordal_triangulator_type::VertexReference;
	using vertex_type = Vertex<value_type>;
	using position_vector_type = typename vertex_type::position_vector_type;
	using klm_vector_type = typename vertex_type::klm_vector_type;
	using segment_triangulator_type = SegmentTriangulator<value_type, index_type>;
	using contour_type = CubicBezierLoop<position_vector_type>;
	using bezier_type = typename segment_triangulator_type::bezier_type;
	static_assert(std::is_same<bezier_type, typename contour_type::bezier_type>::value, "Bezier types missmatch");

	OutlineProcessor(index_type primitiveRestartIndex = ~index_type(0));
	OutlineProcessor(const OutlineProcessor& other) = delete;
	OutlineProcessor(OutlineProcessor&& other) = default;
	~OutlineProcessor() = default;

	OutlineProcessor&						operator=(const OutlineProcessor& other) = delete;
	OutlineProcessor&						operator=(OutlineProcessor&& other) = default;

	void									clear();

	void									addBezier(	const bezier_type& bezier, 
														FillSide fillSide = FillSide::LEFT);
	void									addPolygon(const polygon_type& polygon);
	void									addContour(const contour_type& contour);
	void									addOutline(Utils::BufferView<const contour_type> outline);

	Utils::BufferView<const vertex_type>	getVertices() const noexcept;
	Utils::BufferView<const index_type>		getIndices() const noexcept;

	
private:
	index_type 								m_primitiveRestartIndex;

	std::vector<vertex_type>				m_vertices;
	std::vector<index_type>					m_indices;

	contour_type							m_ccwContour;
	polygon_type							m_innerHull;
	std::vector<vertex_type>				m_chordalAxis;
	std::vector<vertex_reference_type>		m_innerHullReferences;
	polygon_triangulator_type				m_polygonTriangulator;
	segment_triangulator_type				m_segmentTriangulator;
	
};

}

#include "OutlineProcessor.inl"