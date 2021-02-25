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

		//Determine if the control points are inside or outside to form a quad
		const auto bezierAxis = bezier.getAxis();
		const auto b1IsOutside = std::signbit(getSignedDistance(bezierAxis, bezier[1]));
		const auto b2IsOutside = std::signbit(getSignedDistance(bezierAxis, bezier[2]));

		//The vertices are not going to be drawn in order, so that a triangle strip is formed
		std::array<size_t, bezier.size()> triangleStripMapping;
		if(b1IsOutside && b2IsOutside) {
			triangleStripMapping = { 0, 3, 1, 2 };
		} else if(b1IsOutside && !b2IsOutside) {
			triangleStripMapping = { 0, 2, 1, 3 };
		} else if(!b1IsOutside && b2IsOutside) {
			triangleStripMapping = { 0, 1, 2, 3 };
		} else { //if(!b1IsOutside && !b2IsOutside) {
			triangleStripMapping = { 0, 1, 3, 2 };
		} 

		//Ensure the size of the arrays is correct
		static_assert(bezier.size() == klmCoords.klmCoords.size(), "Sizes must match");
		static_assert(bezier.size() == triangleStripMapping.size(), "Sizes must match");

		//Add all the new vertices to the vertex vector with
		//its corresponding indices
		for(size_t j = 0; j < bezier.size(); ++j) {
			//Simply refer to the following vertex
			m_indices.emplace_back(m_vertices.size()); 

			//Obtain the components of the vertex and add them to the vertex list
			const auto index = triangleStripMapping[j];
			const auto& position = bezier[index];
			const auto& klmCoord = klmCoords.klmCoords[index];

			//Add the new vertices
			m_vertices.emplace_back(position, klmCoord);
		}
	} else if(!std::isnan(klmCoords.subdivisionParameter)) {
		assert(!"Subdivision required!"); //TODO
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