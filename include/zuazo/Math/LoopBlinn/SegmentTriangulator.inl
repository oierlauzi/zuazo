#include "SegmentTriangulator.h"

#include "../Triangulator.h"

#include <algorithm>
#include <cassert>

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I>
constexpr SegmentTriangulator<T, I>::VertexData::VertexData(const position_vector_type& position,
															const klm_vector_type& klm,
															bool isProtruding,
															index_type helperIndex ) noexcept
	: vertex(position, klm)
	, isProtruding(isProtruding)
	, helperIndex(helperIndex)
{
}



template<typename T, typename I>
inline SegmentTriangulator<T, I>::Result::Result(	Utils::BufferView<const VertexData> vertices, 
													Utils::BufferView<const index_type> indices ) noexcept
{
	setVertices(vertices);
	setIndices(indices);
}


template<typename T, typename I>
inline void SegmentTriangulator<T, I>::Result::setVertexCount(size_t count) noexcept {
	assert(count <= m_vertices.max_size());
	m_vertexCount = count;
}

template<typename T, typename I>
inline size_t SegmentTriangulator<T, I>::Result::getVertexCount() const noexcept {
	return m_vertexCount;
}


template<typename T, typename I>
inline void SegmentTriangulator<T, I>::Result::setIndexCount(size_t count) noexcept {
	assert(count <= m_indices.max_size());
	m_indexCount = count;
}

template<typename T, typename I>
inline size_t SegmentTriangulator<T, I>::Result::getIndexCount() const noexcept {
	return m_indexCount;
}


template<typename T, typename I>
void SegmentTriangulator<T, I>::Result::setVertices(Utils::BufferView<const VertexData> vert) noexcept {
	setVertexCount(vert.size());
	std::copy(vert.cbegin(), vert.cend(), m_vertices.begin());
}

template<typename T, typename I>
inline Utils::BufferView<typename SegmentTriangulator<T, I>::VertexData>
SegmentTriangulator<T, I>::Result::getVertices() noexcept {
	return Utils::BufferView<VertexData>(m_vertices.data(), getVertexCount());
}

template<typename T, typename I>
inline Utils::BufferView<const typename SegmentTriangulator<T, I>::VertexData>
SegmentTriangulator<T, I>::Result::getVertices() const noexcept {
	return Utils::BufferView<const VertexData>(m_vertices.data(), getVertexCount());
}


template<typename T, typename I>
void SegmentTriangulator<T, I>::Result::setIndices(Utils::BufferView<const index_type> ind) noexcept {
	setIndexCount(ind.size());
	std::copy(ind.cbegin(), ind.cend(), m_indices.begin());
}

template<typename T, typename I>
inline Utils::BufferView<typename SegmentTriangulator<T, I>::index_type>
SegmentTriangulator<T, I>::Result::getIndices() noexcept {
	return Utils::BufferView<index_type>(m_indices.data(), getIndexCount());
}

template<typename T, typename I>
inline Utils::BufferView<const typename SegmentTriangulator<T, I>::index_type>
SegmentTriangulator<T, I>::Result::getIndices() const noexcept {
	return Utils::BufferView<const index_type>(m_indices.data(), getIndexCount());
}



template<typename T, typename I>
inline typename SegmentTriangulator<T, I>::Result
SegmentTriangulator<T, I>::operator()(	const bezier_type& bezier,
										FillSide fillSide,
										index_type baseIndex,
										index_type restartIndex ) const noexcept 
{
	Result result;

	const auto classification = Classifier<value_type>()(bezier);
	const auto klmCoords = KLMCalculator<value_type>()(classification, fillSide);

	//Optimize away lines and points as they have been drawn at the inner hull
	if(!klmCoords.isLineOrPoint && std::isnan(klmCoords.subdivisionParameter)) {
		//We're going to draw a quad (or a tri with a center vertex). Add those vertices
		result.setVertexCount(bezier.size());
		assert(result.getVertices().size() == bezier.size());
		assert(result.getVertices().size() == klmCoords.klmCoords.size());
		std::transform(
			bezier.cbegin(), bezier.cend(), klmCoords.klmCoords.cbegin(),
			result.getVertices().begin(),
			[] (const position_vector_type& pos, const klm_vector_type& klm) -> VertexData {
				return VertexData(pos, klm, false);
			}
		);
		result.getVertices().front().isProtruding = true;
		result.getVertices().back().isProtruding = true;

		//Obtain the directions of the bezier and its control points
		const auto bezierDir = bezier.back() - bezier.front();
		const auto b1Dir = bezier[1] - bezier.front();
		const auto b2Dir = bezier[2] - bezier.front();

		//Create a rotation matrix to project the control points
		//onto a basis formed by the axis of the segment and its
		//normal. Although the axis we're passing is not normalized,
		//we don't care about the absolute magnitude of it, only if
		//it is positive or negative and some relative comparisons.
		//Therefore, passing the "normalized" parameter will avoid
		//some extra calculations
		const auto bezierBasis = getAlignmentMatrix(bezierDir, normalized);
		const auto b1Proj = transform(bezierBasis, b1Dir);
		const auto b2Proj = transform(bezierBasis, b2Dir);

		//Determine if the control points are inside or outside .
		//Negative values will be outside, as the normal axis
		//points inwards
		const bool b1IsOutside = b1Proj.y < 0;
		const bool b2IsOutside = b2Proj.y < 0;

		//Determine the layout of the vertices, so that although
		//the vertices are not going to be drawn in order, 
		//the triangle strip is well-formed
		if(b1IsOutside == b2IsOutside) {
			//Both control points are on the same side. Suppose they are
			//outside, we'll correct it later if necessary
			//Determine if any of the control points overlap the other
			if(!approxZero(distance2(b1Proj, b2Proj))) {
				//This is a quadratic curve. Just draw a tri
				//b1 will be protruding only if it is inside
				result.getVertices()[1].isProtruding = 	(fillSide==FillSide::LEFT && !b1IsOutside) || 
														(fillSide==FillSide::RIGHT && b1IsOutside) ;

				//Establish the helper vertices
				if(result.getVertices()[1].isProtruding) {
					result.getVertices()[0].helperIndex = 2;
					result.getVertices()[1].helperIndex = 0;
				} else {
					result.getVertices()[0].helperIndex = 1;
				}

				//Remove the second control point as it is not needed anymore
				std::swap(result.getVertices()[2], result.getVertices()[3]);
				result.setVertexCount(3);

				//Add the indices to the result
				constexpr std::array<index_type, 3> indices = {
					0, 1, 2
				};
				result.setIndices(indices);

			} else if(isInsideTriangle(bezierDir, b1Dir, b2Dir)) {
				//b2 is overlapped by b1's triangle
				//b1 will be protruding only if it is inside
				result.getVertices()[1].isProtruding = 	(fillSide==FillSide::LEFT && !b1IsOutside) || 
														(fillSide==FillSide::RIGHT && b1IsOutside) ;

				//In order to this triangulation to work b1 needs to be on top of b2
				if(!b1IsOutside) {
					std::swap(result.getVertices()[1], result.getVertices()[2]);

					//Establish the helper vertex (b2)
					result.getVertices()[0].helperIndex = 1;
					if(result.getVertices()[2].isProtruding) {
						result.getVertices()[2].helperIndex = 1;
					}
				} else {
					//Establish the helper vertex (b2)
					result.getVertices()[0].helperIndex = 2;
					if(result.getVertices()[1].isProtruding) {
						result.getVertices()[1].helperIndex = 2;
					}
				}

				//Add the indices to the result
				constexpr std::array<index_type, 5> indices = {
					0, 1, 2, 3, 0
				};
				result.setIndices(indices);

			} else if(isInsideTriangle(bezierDir, b2Dir, b1Dir)) {
				//b1 is overlapped by b2's triangle
				//b2 will be protruding only if it is inside
				result.getVertices()[2].isProtruding =	(fillSide==FillSide::LEFT && !b2IsOutside) || 
														(fillSide==FillSide::RIGHT && b2IsOutside) ;

				//In order to this triangulation to work b2 needs to be on top of b1
				if(!b2IsOutside) {
					std::swap(result.getVertices()[1], result.getVertices()[2]);

					//Establish the helper vertex (b1)
					result.getVertices()[0].helperIndex = 2;
					if(result.getVertices()[1].isProtruding) {
						result.getVertices()[1].helperIndex = 2;
					}
				} else {
					//Establish the helper vertex (b1)
					result.getVertices()[0].helperIndex = 1;
					if(result.getVertices()[2].isProtruding) {
						result.getVertices()[2].helperIndex = 1;
					}
				}
				
				//Add the indices to the result
				constexpr std::array<index_type, 5> indices = {
					3, 0, 1, 2, 3
				};
				result.setIndices(indices);

			} else {
				//Forming a quad
				//Both will be protruding if they are inside. We have them in order in the
				//quad vertices
				result.getVertices()[1].isProtruding = 	(fillSide==FillSide::LEFT && !b1IsOutside) || 
														(fillSide==FillSide::RIGHT && b1IsOutside) ;
				result.getVertices()[2].isProtruding = result.getVertices()[1].isProtruding;

				//Remove the possible self intersection
				if(b1Proj.x > b2Proj.x) {
					std::swap(result.getVertices()[1], result.getVertices()[2]);
				}

				//Establish the helper vertices
				if(result.getVertices()[1].isProtruding) {
					assert(result.getVertices()[2].isProtruding);
					result.getVertices()[0].helperIndex = 3;
					result.getVertices()[1].helperIndex = 3;
					result.getVertices()[2].helperIndex = 0;
				} else {
					assert(!result.getVertices()[2].isProtruding);
					result.getVertices()[0].helperIndex = 2;
				}

				//Triangulate the quad and add the indices to the result
				const auto indices = Triangulator<value_type, index_type>::triangulateQuad(
					triangleStrip,
					result.getVertices()[0].vertex.pos, 
					result.getVertices()[1].vertex.pos, 
					result.getVertices()[2].vertex.pos, 
					result.getVertices()[3].vertex.pos, 
					0
				);
				result.setIndices(indices);

			}

		} else {
			//Sides missmatch. Suppose b1 is outside and b2 inside. We'll correct it later
			//if necessary. 

			//b1 will be protruding only if it is inside, b2 otherwise
			if((fillSide==FillSide::LEFT && !b1IsOutside) || (fillSide==FillSide::RIGHT && b1IsOutside)) {
				result.getVertices()[1].isProtruding = true;
			} else {
				result.getVertices()[2].isProtruding = true;
			}

			//Determine if any of the control points overlaps the extremus
			//points.
			if(isInsideTriangle(b1Dir-bezierDir, b2Dir-bezierDir, -bezierDir)) {				
				//Origin is overlapped by the control points

				assert(false); //TODO test this case
				//Establish the helper vertices
				result.getVertices()[0].helperIndex = 3;
				if(result.getVertices()[1].isProtruding) {
					result.getVertices()[1].helperIndex = 0;
				} else {
					assert(result.getVertices()[2].isProtruding);
					result.getVertices()[2].helperIndex = 0;
				}

				//Add the indices to the result
				constexpr std::array<index_type, 5> indices = {
					3, 2, 0, 1, 3
				};
				result.setIndices(indices);

			} else if(isInsideTriangle(b1Dir, b2Dir, bezierDir)) {
				//End is overlapped by the control points

				assert(false); //TODO test this case
				//Establish the helper vertices
				result.getVertices()[0].helperIndex = 3;
				if(result.getVertices()[1].isProtruding) {
					result.getVertices()[1].helperIndex = result.getVertices()[0].helperIndex;
				} else {
					assert(result.getVertices()[2].isProtruding);
					result.getVertices()[2].helperIndex = result.getVertices()[0].helperIndex;
				}
				//Add the indices to the result
				constexpr std::array<index_type, 5> indices = {
					0, 1, 3, 2, 0
				};
				result.setIndices(indices);

			} else {
				//Forming a quad
				//Establish the helper vertices
				result.getVertices()[0].helperIndex = 3;
				if(result.getVertices()[1].isProtruding) {
					result.getVertices()[1].helperIndex = 0;
				} else {
					assert(result.getVertices()[2].isProtruding);
					result.getVertices()[2].helperIndex = 0;
				}

				//Remove the self intersection
				const auto swapVertices = 	b1IsOutside ? 
											std::make_pair(2, 3) :
											std::make_pair(0, 1) ;
				auto quadVertices = bezier;
				std::swap(quadVertices[swapVertices.first], quadVertices[swapVertices.second]);

				//Triangulate the quad and add the indices to the result
				auto indices = Triangulator<value_type, index_type>::triangulateQuad(
					triangleStrip,
					quadVertices[0], 
					quadVertices[1], 
					quadVertices[2], 
					quadVertices[3], 
					0
				);

				//Undo the previous swapping, as it hasn't been done in the actual result
				std::iter_swap(
					std::find(indices.begin(), indices.end(), swapVertices.first),
					std::find(indices.begin(), indices.end(), swapVertices.second)
				);

				//Add the indices to the result
				result.setIndices(indices);

			}

		}

		//If the b1 control point of the curve turns out to be inwards, 
		//as opposed to our supposition.
		if(!b1IsOutside) {
			if(result.getVertexCount() == bezier.size()) {
				//4 vertices, set complemetary values
				for(auto& index : result.getIndices()) {
					index = bezier.degree() - index;
				}
			} else {
				//3 vertices, reverse them
				std::reverse(result.getIndices().begin(), result.getIndices().end());
			}
		}

		//Apply the base index
		for(auto& index : result.getIndices()) {
			index += baseIndex;
		}

		for(auto& vertexData : result.getVertices()) {
			if(vertexData.helperIndex < result.getVertices().size()) {
				vertexData.helperIndex += baseIndex;
			}
		}

	} else if(!std::isnan(klmCoords.subdivisionParameter)) {
		//This is the special case where the loop has 2 solutions 
		//in [0, 1] for the equation, and a rendering artifact appears.
		//We'll split the curve at that point and add each half separately.
		const auto halves = split(bezier, klmCoords.subdivisionParameter);

		//Process each half
		const auto result0 = (*this)(
			halves.front(),
			fillSide,
			baseIndex,
			restartIndex
		);
		const auto result1 = (*this)(
			halves.back(),
			(fillSide==FillSide::LEFT) ? FillSide::RIGHT : FillSide::LEFT, //Invert the fill-side
			baseIndex + result0.getVertices().size()-1, //Accumulate the offset. Well ignore the last vertex of result0, so -1
			restartIndex
		);

		//Copy the data to the results
		//TODO this will fail in case of a self-intersecting loop
		result.setVertexCount(result0.getVertexCount() + result1.getVertexCount() - 1); //Account for middle vertice's redundancy.
		auto lastVertex = std::copy(
			result0.getVertices().cbegin(), std::prev(result0.getVertices().cend()), //Middle vertex provided by result1
			result.getVertices().begin()
		);
		lastVertex = std::copy(
			result1.getVertices().cbegin(), result1.getVertices().cend(), 
			lastVertex
		);
		assert(lastVertex == result.getVertices().cend());

		result.setIndexCount(result0.getIndexCount() + result1.getIndexCount() + 1);
		auto lastIndex = std::copy(
			result0.getIndices().cbegin(), result0.getIndices().cend(), 
			result.getIndices().begin()
		);
		*(lastIndex++) = restartIndex; //Restart the primitive in-between
		lastIndex = std::copy(
			result1.getIndices().cbegin(), result1.getIndices().cend(), 
			lastIndex
		);
		assert(lastIndex == result.getIndices().cend());

	} else {
		assert(klmCoords.isLineOrPoint);

		if(approxZero(distance2(bezier.front(), bezier.back()))) {
			//This is a line. We are not drawing anything,
			//but we'll add the points in case they are needed.
			//However, no indices will be generated. Moreover, 
			//helper index will be invalid, despite being signaled
			//as a protruding vertex.
			const std::array<VertexData, 2> vertices = {
				VertexData(bezier.front(), klmCoords.klmCoords.front(), true, ~index_type(0)),
				VertexData(bezier.back(),  klmCoords.klmCoords.back(),  true, ~index_type(0))
			};

			result.setVertices(vertices);
		}	
	}

	return result;
}

}