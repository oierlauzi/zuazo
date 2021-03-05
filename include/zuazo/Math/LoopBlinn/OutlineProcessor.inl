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
inline typename OutlineProcessor<T, I>::BezierResult
OutlineProcessor<T, I>::addBezier(	const bezier_type& bezier,
									FillSide fillSide ) 
{
	BezierResult result;
	result.protudingVertices = {
		bezier.front(),
		bezier.front()
	};

	const auto classification = Classifier<value_type>()(bezier);
	auto klmCoords = KLMCalculator<value_type>()(classification, fillSide);

	//Optimize away lines and points as they have been drawn at the inner hull
	if(!klmCoords.isLineOrPoint && std::isnan(klmCoords.subdivisionParameter)) {
		//Something is going to be drawn, reset the primitive assembly
		if(!m_indices.empty()) {
			m_indices.emplace_back(m_primitiveRestartIndex);
		}

		const auto alignedBezier = alignToAxis(bezier);
		assert(alignedBezier.front() == position_vector_type());

		//Determine if the control points are inside or outside .
		//Negative values will be outside, as the normal axis
		//points inwards
		const bool b1IsOutside = alignedBezier[1].y < 0;
		const bool b2IsOutside = alignedBezier[2].y < 0;

		//Determine the layout of the vertices, so that although
		//the vertices are not going to be drawn in order, 
		//the triangle strip is well-formed
		auto quadVertices = bezier;
		std::array<index_type, bezier.size()> bezierOrdering;
		bool repeatFirstVertex = false;

		if(b1IsOutside == b2IsOutside) {
			//Both control points are on the same side. Suppose they are
			//outside, we'll correct it later if necessary
			//Determine if any of the control points overlap the other
			if(!approxZero(distance2(alignedBezier[1], alignedBezier[2]))) {
				//Both control points are in the same spot. This is a quadratic
				//curve. Skip the second control point. (Only 1 tri)
				bezierOrdering = { 0, 0, 1, 3 };

				if((fillSide==FillSide::LEFT && !b1IsOutside) || (fillSide==FillSide::RIGHT && b1IsOutside)) {
					result.protudingVertices[0] = bezier[1];
				}
			} else if(isInsideTriangle(alignedBezier.back(), alignedBezier[1], alignedBezier[2])) {
				//b2 is overlapped by b1's triangle
				//In order to this triangulation to work b1 needs to be on top of b2
				enum { SWAP_A = 1, SWAP_B = 2 };
				if(!b1IsOutside) {
					std::swap(quadVertices[SWAP_A], quadVertices[SWAP_B]);
					std::swap(klmCoords.klmCoords[SWAP_A], klmCoords.klmCoords[SWAP_B]);
				}

				bezierOrdering = { 0, 1, 2, 3 };
				repeatFirstVertex = true; //We'll draw 3 triangles	

				//b1 will be protruding only if it is inside
				if((fillSide==FillSide::LEFT && !b1IsOutside) || (fillSide==FillSide::RIGHT && b1IsOutside)) {
					result.protudingVertices[0] = bezier[1];
				}

			} else if(isInsideTriangle(alignedBezier.back(), alignedBezier[2], alignedBezier[1])) {
				//b1 is overlapped by b2's triangle
				//In order to this triangulation to work b2 needs to be on top of b1
				enum { SWAP_A = 1, SWAP_B = 2 };
				if(!b1IsOutside) {
					std::swap(quadVertices[SWAP_A], quadVertices[SWAP_B]);
					std::swap(klmCoords.klmCoords[SWAP_A], klmCoords.klmCoords[SWAP_B]);
				}

				bezierOrdering = { 3, 0, 1, 2 };
				repeatFirstVertex = true; //We'll draw 3 triangles

				//b2 will be protruding only if it is inside
				if((fillSide==FillSide::LEFT && !b2IsOutside) || (fillSide==FillSide::RIGHT && b2IsOutside)) {
					result.protudingVertices[0] = bezier[2];
				}

			} else {
				//Forming a quad
				enum { SWAP_A = 1, SWAP_B = 2 };

				//Remove the possible self intersection
				const auto crossing = alignedBezier[1].x > alignedBezier[2].x;
				if(crossing) {
					std::swap(quadVertices[SWAP_A], quadVertices[SWAP_B]);
					std::swap(klmCoords.klmCoords[SWAP_A], klmCoords.klmCoords[SWAP_B]);
				}

				//Triangulate the quad
				bezierOrdering = m_triangulator.triangulateQuad(
					quadVertices[0], 
					quadVertices[1], 
					quadVertices[2], 
					quadVertices[3], 
					0
				);

				//We do not need to repeat the first vertex
				repeatFirstVertex = false;

				//Both will be protruding if they are inside. We have them in order in the
				//quad vertices
				if((fillSide==FillSide::LEFT && !b1IsOutside) || (fillSide==FillSide::RIGHT && b1IsOutside)) {
					result.protudingVertices = { quadVertices[1], quadVertices[2] };
				}
			}

		} else {
			//Sides missmatch. Suppose b1 is outside and b2 inside. We'll correct it later
			//if necessary. Determine if any of the control points overlaps the extremus
			//points. This should not happen, as this setup involves self intersection
			//of the curve (loop). However, I've decided to check it, as it will avoid
			//crashing due to non-convex quads in the else branch.
			if(isInsideTriangle(alignedBezier[1]-alignedBezier.back(), alignedBezier[2]-alignedBezier.back(), -alignedBezier.back())) {
				//origin is overlapped by the control points
				bezierOrdering = { 3, 2, 0, 1 };
				repeatFirstVertex = true; //We'll draw 3 triangles

			} else if(isInsideTriangle(alignedBezier[1], alignedBezier[2], alignedBezier.back())) {
				//b1 is overlapped by b2's triangle
				bezierOrdering = { 0, 1, 3, 2 };
				repeatFirstVertex = true; //We'll draw 3 triangles

			} else {
				//Forming a quad
				enum { SWAP_A = 2, SWAP_B = 3 };

				//Remove the self intersection
				std::swap(quadVertices[SWAP_A], quadVertices[SWAP_B]);
				std::swap(klmCoords.klmCoords[SWAP_A], klmCoords.klmCoords[SWAP_B]);

				//Triangulate the quad
				bezierOrdering = m_triangulator.triangulateQuad(
					quadVertices[0], 
					quadVertices[1], 
					quadVertices[2], 
					quadVertices[3], 
					0
				);

				//We do not need to repeat the first vertex
				repeatFirstVertex = false;

			}

			//b1 will be protruding only if it is inside, b2 otherwise
			if((fillSide==FillSide::LEFT && !b1IsOutside) || (fillSide==FillSide::RIGHT && b1IsOutside)) {
				result.protudingVertices[0] = bezier[1];
			} else {
				result.protudingVertices[0] = bezier[2];
			}
		}

		//If the b1 control point of the curve turns out to be inwards, 
		//as opposed to our supposition.
		if(!b1IsOutside) {
			for(auto& i : bezierOrdering) {
				i = bezier.degree() - i;
			}	
		}

		//Ensure the size of the arrays is correct
		static_assert(quadVertices.size() == klmCoords.klmCoords.size(), "Sizes must match");
		static_assert(quadVertices.size() == bezierOrdering.size(), "Sizes must match");

		//Add all the new vertices to the vertex vector with
		//its corresponding indices
		for(size_t j = 0; j < quadVertices.size(); ++j) {
			//Skip repeated vertices
			const auto index = bezierOrdering[j];
			if(j > 0) {
				if(bezierOrdering[j-1] == index) {
					continue;
				}
			}

			//Simply refer to the following vertex
			m_indices.emplace_back(m_vertices.size()); 

			//Obtain the components of the vertex and add them to the vertex list
			const auto& position = quadVertices[index];
			const auto& klmCoord = klmCoords.klmCoords[index];
			m_vertices.emplace_back(position, klmCoord);
		}

		//Repeat an the first index if necessary
		if(repeatFirstVertex) {
			m_indices.emplace_back(m_vertices.size() - quadVertices.size());
		}

	} else if(!std::isnan(klmCoords.subdivisionParameter)) {
		//This is the special case where the loop has 2 solutions 
		//in [0, 1] for the equation, and a rendering artifact appears.
		//We'll split the curve at that point and add each half separately.
		const auto halves = split(bezier, klmCoords.subdivisionParameter);

		//As we've made 2 halves, there is a unfilled triangle in the middle
		//Determine if the empty triangle would lies
		//or right side of the left side
		const auto bezierDirection = bezier.back() - bezier.front();
		const auto middleDirection = halves.front().back() - bezier.front();
		const auto middleSide = zCross(bezierDirection, middleDirection);

		//Determine if the triangle should be filled, and if
		//so, add a protruding vertex to indicate to do so
		if(	(fillSide == FillSide::LEFT && middleSide < 0) ||
			(fillSide == FillSide::RIGHT && middleSide > 0) ) 
		{
			result.protudingVertices[0] = halves.front().back();
		}

		//Add both sections
		for(const auto& half : halves) {
			addBezier(half, fillSide);
			reinterpret_cast<std::underlying_type<FillSide>::type&>(fillSide) ^= 1; //Exchanges 1<=>0
		}
	}

	return result;
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
	m_innerHull.clear();
	for(size_t i = 0; i < m_ccwContour.getSegmentCount(); ++i) {
		const auto& bezier = m_ccwContour.getSegment(i);

		//Add the outline. As we're CCW, fill the left side
		const auto addResult = addBezier(bezier, FillSide::LEFT);

		//First point will always be at the inner hull
		m_innerHull.lineTo(bezier.front());

		//Add all the points that protrude the inner hull
		for(const auto& point : addResult.protudingVertices) {
			if(point == bezier.front()) {
				break;
			} else {
				m_innerHull.lineTo(point);
			}
		}
	}

	//Add the inner hull as a polygon
	assert(getSignedArea(m_innerHull) >= 0);
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