#include "OutlineProcessor.h"

#include "Classifier.h"
#include "KLMCalculator.h"

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I>
inline OutlineProcessor<T, I>::OutlineProcessor(index_type primitiveRestartIndex)
	: m_primitiveRestartIndex(primitiveRestartIndex)
{
}



template<typename T, typename I>
inline void OutlineProcessor<T, I>::clear() {
	m_vertices.clear();
	m_indices.clear();
}


template<typename T, typename I>
inline void OutlineProcessor<T, I>::addBezier(const bezier_type& bezier) {
	const auto classification = Classifier<value_type>()(bezier);
	const auto klmCoords = KLMCalculator<value_type>()(classification, FillSide::LEFT);

	//Optimize away lines and points as they have been drawn at the inner hull
	if(!klmCoords.isLineOrPoint && std::isnan(klmCoords.subdivisionParameter)) {
		//Something is going to be drawn, reset the primitive assembly
		if(!m_indices.empty()) {
			m_indices.emplace_back(m_primitiveRestartIndex);
		}

		//Obtain the 2D basis for the bezier curve's axis. Note that the basis is
		//orthogonal but not normalized. X axis is parallel to the curve's axis,
		//whilst the Y axis points outwards
		const auto bezierAxis = bezier.back() - bezier.front();
		const auto bezierNormal = position_vector_type(-bezierAxis.y, bezierAxis.x);
		const auto bezierBasis = inv(Mat2x2<value_type>(bezierAxis, bezierNormal));

		//Remove the offset part from the control points
		const auto b1Axis = bezier[1] - bezier.front();
		const auto b2Axis = bezier[2] - bezier.front();

		//Project the 2 control points into the basis. //FIXME
		const auto b1Proj = operator*<value_type, 2, 2>(bezierBasis, b1Axis);
		const auto b2Proj = operator*<value_type, 2, 2>(bezierBasis, b2Axis);

		//Determine if the control points are inside or outside 
		const auto b1IsOutside = !std::signbit(b1Proj.y);
		const auto b2IsOutside = !std::signbit(b2Proj.y);

		//Determine the layout of the vertices, so that although
		//the vertices are not going to be drawn in order, 
		//the triangle strip is well-formed
		auto quadVertices = reinterpret_cast<const std::array<position_vector_type, bezier.size()>&>(bezier);
		std::array<index_type, bezier.size()> quadIndices;
		bool repeatFirstvertex = false;

		if(b1IsOutside == b2IsOutside) {
			//Both control points are on the same side. Suppose they are
			//outside, we'll correct it later if necessary
			//Determine if any of the control points overlaps the other
			if(isInsideTriangle(bezierAxis, b1Axis, b2Axis)) {
				//b2 is overlapped by b1's triangle
				quadIndices = { 0, 3, 2, 1 };
				repeatFirstvertex = true; //We'll draw 3 triangles
			} else if(isInsideTriangle(bezierAxis, b2Axis, b1Axis)) {
				//b1 is overlapped by b2's triangle
				quadIndices = { 0, 3, 1, 2 };
				repeatFirstvertex = true; //We'll draw 3 triangles
			} else {
				//Forming a quad
				//Remove the possible self intersection
				const auto crossing = b1Proj.x > b2Proj.x;
				if(crossing) {
					std::swap(quadVertices[1], quadVertices[2]);
				}

				//Triangulate the quad
				quadIndices = m_triangulator.triangulateQuad(quadVertices, 0);
			}

		} else {
			//Sides missmatch. Suppose b1 is outside and b2 inside. We'll correct it later
			//if necessary
			//Determine if any of the control points overlaps the extremus points
			if(isInsideTriangle(b1Axis, b2Axis, position_vector_type())) {
				//origin is overlapped by the control points
				quadIndices = { 3, 2, 0, 1 };
				repeatFirstvertex = true; //We'll draw 3 triangles
			} else if(isInsideTriangle(b1Axis, b2Axis, bezierAxis)) {
				//b1 is overlapped by b2's triangle
				quadIndices = { 1, 0, 3, 2 };
				repeatFirstvertex = true; //We'll draw 3 triangles
			} else {
				//Forming a quad
				//Remove the self intersection
				std::swap(quadVertices[0], quadVertices[3]);
				std::swap(quadVertices[3], quadVertices[2]);

				//Triangulate the quad
				quadIndices = m_triangulator.triangulateQuad(quadVertices, 0);

			}

		}

		//If the b1 control point of the curve turns out to be inwards, 
		//as opposed to our supposition, reverse it by pairs
		if(!b1IsOutside) {
			std::iter_swap(
				std::find(quadIndices.begin(), quadIndices.end(), 0),
				std::find(quadIndices.begin(), quadIndices.end(), 3)
			);
			std::iter_swap(
				std::find(quadIndices.begin(), quadIndices.end(), 1),
				std::find(quadIndices.begin(), quadIndices.end(), 2)
			);
		}

		//Ensure the size of the arrays is correct
		static_assert(quadVertices.size() == klmCoords.klmCoords.size(), "Sizes must match");
		static_assert(quadVertices.size() == quadIndices.size(), "Sizes must match");

		//Add all the new vertices to the vertex vector with
		//its corresponding indices
		for(size_t j = 0; j < quadVertices.size(); ++j) {
			//Simply refer to the following vertex
			m_indices.emplace_back(m_vertices.size()); 

			//Obtain the components of the vertex and add them to the vertex list
			const auto index = quadIndices[j];
			const auto& position = quadVertices[index];
			const auto& klmCoord = klmCoords.klmCoords[index];

			//Add the new vertices
			m_vertices.emplace_back(position, klmCoord);
		}

		//Repeat an the first index if necessary
		if(repeatFirstvertex) {
			m_indices.emplace_back(m_vertices.size() - quadIndices.size());
		}

	} else if(!std::isnan(klmCoords.subdivisionParameter)) {
		//This is the special case where the loop has 2 solutions 
		//in [0, 1] for the equation, and a rendering artifact appears.
		//We'll split the curve at that point and add each half separately.
		const auto halves = split(bezier, klmCoords.subdivisionParameter);

		//Add both sections
		for(const auto& half : halves) {
			addBezier(half);
		}
	}
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addPolygon(const polygon_type& polygon) {
	//Triangulate the new vertices and append them to the indices
	m_triangulator(m_indices, polygon, m_vertices.size(), m_primitiveRestartIndex);

	//Insert all the new vertices with the fill-all klm values	
	m_vertices.insert(m_vertices.cend(), polygon.cbegin(), polygon.cend());
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addContour(const contour_type& contour) {
	//Ensure that the contour is counter clockwise
	m_ccwContour = contour;
	if(getSignedArea(reinterpret_cast<const polygon_type&>(m_ccwContour)) > 0) { //FIXME ugly reinterpret cast
		m_ccwContour.reverse();
	}
	assert(getSignedArea(reinterpret_cast<const polygon_type&>(m_ccwContour)) <= 0);

	//Obtain the vertices corresponding to the inner hull
	m_innerHull.clear();
	for(size_t i = 0; i < m_ccwContour.getSegmentCount(); ++i) {
		const auto& segment = m_ccwContour.getSegment(i);
		const auto segmentAxis = segment.getAxis();

		//First point will always be at the inner hull
		m_innerHull.lineTo(segment.front());

		//Evaluate it for the 2 control points
		for(size_t j = 1; j < segment.degree(); ++j) {
			const auto sDist = getSignedDistance(segmentAxis, segment[j]);

			if(sDist < 0) {
				//This segment lies on the inside
				m_innerHull.lineTo(segment[j]);
			}
		}

		//Add the outline
		addBezier(segment);
	}

	//Add the inner hull as a polygon
	addPolygon(m_innerHull);
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addOutline(Utils::BufferView<const contour_type> outline) {
	for(const auto& contour : outline) {
		addContour(contour);
	}
}

template<typename T, typename I>
inline Utils::BufferView<const typename OutlineProcessor<T, I>::Vertex> OutlineProcessor<T, I>::getVertices() const noexcept {
	return m_vertices;
}

template<typename T, typename I>
inline Utils::BufferView<const typename OutlineProcessor<T, I>::index_type> OutlineProcessor<T, I>::getIndices() const noexcept {
	return m_indices;
}

}