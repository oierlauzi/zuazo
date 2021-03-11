#include "OutlineProcessor.h"

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
inline void OutlineProcessor<T, I>::addBezier(	const bezier_type& bezier,
												FillSide fillSide ) 
{
	//Triangulate this segment
	const auto bezierTriangulation = m_segmentTriangulator(
		bezier,
		fillSide,
		m_vertices.size(),
		m_primitiveRestartIndex
	);
	const auto vertices = bezierTriangulation.getVertices();
	const auto indices = bezierTriangulation.getIndices();

	//Copy its vertices to the vertex list
	std::transform(
		vertices.cbegin(),
		vertices.cend(),
		std::back_inserter(m_vertices),
		[] (const typename segment_triangulator_type::VertexData& vertexData) -> vertex_type {
			return vertexData.vertex;
		}
	);

	//Restart a new primitive if necessary
	if(!m_indices.empty() && !indices.empty()) {
		m_indices.emplace_back(m_primitiveRestartIndex);
	}

	//Copy the indices
	std::copy(
		indices.cbegin(),
		indices.cend(),
		std::back_inserter(m_indices)
	);
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addPolygon(const polygon_type& polygon) {
	//Triangulate the new vertices and append them to the indices
	m_polygonTriangulator(
		polygon, 		
		typename polygon_triangulator_type::TriangleStripGenerator(
			m_indices, m_vertices.size(), m_primitiveRestartIndex
		)
	);

	//Insert all the new vertices with the fill-all klm values	
	m_vertices.insert(m_vertices.cend(), polygon.cbegin(), polygon.cend());
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addContour(const contour_type& contour) {
	//Reinterpret the contour as a polygon. This works as both types have
	//A single vector of value_type-s as a member. The contour has the
	//additional limitation of having 3N+1 elements. (Polygon should have N+1).
	//HACK Ugly reinterpret cast
	const auto& ccwContourAsPolygon = reinterpret_cast<const polygon_type&>(m_ccwContour);

	//Ensure that the contour is counter clockwise
	//FIXME it is more correct to check CCW-ness only with the front-back points,
	//as crossing control points may confuse this check
	m_ccwContour = contour;
	if(getSignedArea(ccwContourAsPolygon) < 0) { 
		m_ccwContour.reverse();
	}
	assert(getSignedArea(ccwContourAsPolygon) >= 0);

	//Remove all intersections between control points and axes
	for(size_t i = 0; i < m_ccwContour.getSegmentCount(); ++i) {
		//Get the axis of the segment
		auto segment0Axis = m_ccwContour.getSegment(i).getAxis();

		for(size_t j = 0; j < m_ccwContour.getSegmentCount(); ++j) {
			//Obtain the indices of the next and previous j
			const auto jp1 = j < m_ccwContour.getSegmentCount()-1 ? j+1 : 0;
			const auto jm1 = j > 0 ? j-1 : m_ccwContour.getSegmentCount()-1;
			assert(jp1 < m_ccwContour.getSegmentCount());
			assert(jm1 < m_ccwContour.getSegmentCount());

			//Avoid checking with itself and neighbors.
			//FIXME this will not work well with 2-segment curves
			//FIXME check intersection with control points
			if(i == jm1 || i == j || i == jp1) {
				continue;
			}

			//Ensure that there are no intersecting segments and
			//control points
			bool intersection;
			do {
				const auto& segment1 = m_ccwContour.getSegment(j);
				auto segment1Axis = segment1.getAxis();
				auto b1Axis = Line<value_type, 2>(segment1.front(), segment1[1]);
				auto b2Axis = Line<value_type, 2>(segment1.back(), segment1[2]);

				//Ensure that the axes don't self intersect, as this
				//means that we have a complex polygon which cannot be 
				//triangulated
				if(getIntersection(segment0Axis, segment1Axis)) {
					throw Exception("Self intersecting contour provided");
				}

				//Check if any of the control points intersect the bezier axis
				intersection = 	getIntersection(segment0Axis, b1Axis) || 
								getIntersection(segment0Axis, b2Axis) ;


				//If this occurs, split both parts. Repeat until the intersection
				//disapears (or turns out to be a self intersection)
				if(intersection) {
					//Split in 2, considering that after doing so,
					//a new segment is introduced, so the index needs
					//to consider it.
					//TODO split more optimally (t value calculation)
					auto& a = i>j ? i : j; //Highest index (needs to be incremented later)
					auto& b = i>j ? j : i; //Lowest index (keeps its value)
					m_ccwContour.split(a++, value_type(0.5)); 
					m_ccwContour.split(b  , value_type(0.5));

					assert(i < m_ccwContour.size());
					assert(j < m_ccwContour.size());
					assert(i!=j);

					//Recaclulate segment0Axis, as it has been modified
					//when splitting
					segment0Axis = m_ccwContour.getSegment(i).getAxis();
				}
			} while(intersection);
		}
	}

	//Obtain the vertices corresponding to the inner hull
	assert(m_innerHull.size() == 0);
	assert(m_innerHullReferences.size() == 0);
	for(size_t i = 0; i < m_ccwContour.getSegmentCount(); ++i) {
		//Triangulate this segment
		const auto baseIndex = m_vertices.size();
		const auto bezierTriangulation = m_segmentTriangulator(
			m_ccwContour.getSegment(i),
			FillSide::LEFT,
			baseIndex,
			m_primitiveRestartIndex
		);
		const auto vertices = bezierTriangulation.getVertices();
		const auto indices = bezierTriangulation.getIndices();

		//Add the protruding vertices (except the last one) to the inner hull.
		//The last one will be provided by the next segment.
		for(size_t j = 0; j < vertices.size() - 1; ++j) {
			const auto& vertexData = vertices[j];
			if(vertexData.isProtruding) {
				//Add the vertex to the inner hull
				m_innerHull.lineTo(vertexData.vertex.pos);
				
				//Add its reference to reference it from the inner hull
				const auto vertexIndex = baseIndex + j;
				const auto helperIndex = vertexData.helperIndex;
				if(j == 0) {
					//This is the first vertex, which has 2 references.
					//Note that the first one will be incorrect, as it 
					//actually refers to the last one. We'll correct it 
					//at the end
					m_innerHullReferences.emplace_back(vertexIndex, vertexIndex-1, helperIndex);
				} else {
					//This is a intermediary vertex. Has no duplicity
					m_innerHullReferences.emplace_back(vertexIndex, vertexIndex, helperIndex);
				}
			}
		}

		//Copy its vertices to the vertex list
		std::transform(
			vertices.cbegin(),
			vertices.cend(),
			std::back_inserter(m_vertices),
			[] (const typename segment_triangulator_type::VertexData& vertexData) -> vertex_type {
				return vertexData.vertex;
			}
		);

		//Restart a new primitive if necessary
		if(!m_indices.empty() && !indices.empty()) {
			m_indices.emplace_back(m_primitiveRestartIndex);
		}

		//Copy the indices
		std::copy(
			indices.cbegin(),
			indices.cend(),
			std::back_inserter(m_indices)
		);

	}

	//Correct the first reference, as it actually refers to the last one
	m_innerHullReferences.front().back = m_vertices.size()-1;

	assert(getSignedArea(m_innerHull) >= 0);
	assert(m_innerHullReferences.size() == m_innerHull.size());

	//Triangulate the new vertices and append them to the indices
	assert(m_chordalAxis.empty());
	m_polygonTriangulator(
		m_innerHull, 
		chordal_triangulator_type(
			m_indices, 				//out: indices
			m_chordalAxis, 			//out: chordal axis
			m_vertices, 			//in: vertices
			m_innerHullReferences,	//in: references to the vertices at the polygon
			m_vertices.size(),		//in: chordal axis base index. We'll append it at the back
			m_primitiveRestartIndex	//in: primitive restart index
		)
	);

	//Insert the vertices corresponding to the chordal axis
	m_vertices.insert(m_vertices.cend(), m_chordalAxis.cbegin(), m_chordalAxis.cend());

	//Clear all for the next call
	m_ccwContour.clear();
	m_innerHull.clear();
	m_innerHullReferences.clear();
	m_chordalAxis.clear();
}

template<typename T, typename I>
inline void OutlineProcessor<T, I>::addOutline(Utils::BufferView<const contour_type> outline) {
	for(const auto& contour : outline) {
		addContour(contour);
	}
}

template<typename T, typename I>
inline Utils::BufferView<const typename OutlineProcessor<T, I>::vertex_type> OutlineProcessor<T, I>::getVertices() const noexcept {
	return m_vertices;
}

template<typename T, typename I>
inline Utils::BufferView<const typename OutlineProcessor<T, I>::index_type> OutlineProcessor<T, I>::getIndices() const noexcept {
	return m_indices;
}

}