#include "SegmentTriangulator.h"

#include "../Triangulator.h"

#include <algorithm>
#include <cassert>

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I>
constexpr SegmentTriangulator<T, I>::VertexData::VertexData(const position_vector_type& position,
															const klm_vector_type& klm,
															VertexType type,
															index_type helperIndex ) noexcept
	: vertex(position, klm)
	, type(type)
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
	constexpr classifier_type classifier;
	const auto classification = classifier(bezier);
	

	return (*this)(
		bezier,
		classification,
		fillSide,
		baseIndex,
		restartIndex
	);
}

template<typename T, typename I>
inline typename SegmentTriangulator<T, I>::Result
SegmentTriangulator<T, I>::operator()(	const bezier_type& bezier, 
										const typename classifier_type::Result& classification,
										FillSide fillSide,
										index_type baseIndex,
										index_type restartIndex ) const noexcept
{
	constexpr klm_calculator_type klmCalculator;
	const auto klmCoords = klmCalculator(classification);

	return (*this)(
		bezier,
		klmCoords,
		fillSide,
		baseIndex,
		restartIndex
	);
}

template<typename T, typename I>
inline typename SegmentTriangulator<T, I>::Result
SegmentTriangulator<T, I>::operator()(	const bezier_type& bezier, 
										const typename klm_calculator_type::Result& klmCoords,
										FillSide fillSide,
										index_type baseIndex,
										index_type restartIndex ) const noexcept
{
	Result result;
	
	switch(klmCoords.degeneratedType) {
	case DegeneratedCurveType::line: {
		//This is a line. We are not drawing anything,
		//but we'll add the points in case they are needed.
		//However, no indices will be generated. Moreover, 
		//helper indices will be invalid, despite being signaled
		//as a first/last vertices
		const std::array<VertexData, 2> vertices = {
			VertexData(bezier.front(), klmCoords.values.front(), VertexType::first, ~index_type(0)),
			VertexData(bezier.back(),  klmCoords.values.back(),  VertexType::last, ~index_type(0))
		};
		result.setVertices(vertices);

		break;
	}
	case DegeneratedCurveType::quadratic: 
	case DegeneratedCurveType::cubic: 
	{
		//Quadratic and cubic curves will be treated
		//equally

		//Check if the curve needs subdivision
		if(!std::isnan(klmCoords.subdivisionParameter)) {
			//This is the special case where the loop has 2 solutions 
			//in [0, 1] for the equation, and a rendering artifact appears.
			//We'll split the curve at that point and add each half separately.
			const auto halves = split(bezier, klmCoords.subdivisionParameter);
			constexpr classifier_type classifier;
			constexpr klm_calculator_type klmCalculator;
			auto lastVertex = result.getVertices().begin();
			auto lastIndex = result.getIndices().begin();

			for(size_t i = 0; i < halves.size(); ++i) {
				const auto& half = halves[i];

				//Classify and calculate KLM coordinates
				const auto classification = classifier(half);
				auto klmCoords2 = klmCalculator(classification);
				klmCoords2.subdivisionParameter = std::numeric_limits<value_type>::quiet_NaN(); //Can't handle more reentrances.
				
				//Invert the orientation for the second half
				if(i) {
					klmCoords2.reverse = !klmCoords2.reverse;
				}

				//Triangulate each half
				const auto tempResult = (*this)(
					half,
					klmCoords2,
					fillSide,
					baseIndex + result.getVertexCount(),
					restartIndex
				);

				//Add its vertices
				assert(tempResult.getVertexCount() <= bezier_type::size());
				result.setVertexCount(result.getVertexCount() + tempResult.getVertexCount());
				lastVertex = std::copy(
					tempResult.getVertices().cbegin(), tempResult.getVertices().cend(), 
					lastVertex
				);

				//Add its indices
				if(result.getIndexCount() > 0 && tempResult.getIndexCount() > 0) {
					result.setIndexCount(result.getIndexCount() + tempResult.getIndexCount() + 1);
					*(lastIndex++) = restartIndex; //Restart the primitive in-between
				} else {
					result.setIndexCount(result.getIndexCount() + tempResult.getIndexCount());
				}
				lastIndex = std::copy(
					tempResult.getIndices().cbegin(), tempResult.getIndices().cend(), 
					lastIndex
				);

			}

			//Ensure everything has been written
			assert(lastVertex == result.getVertices().cend());
			assert(lastIndex == result.getIndices().cend());

		} else {
			//Normal case
			//Evaluate if klm coordinates need to be reversed
			const bool reverse = 	(klmCoords.reverse && fillSide == FillSide::right) ||
									(!klmCoords.reverse && fillSide == FillSide::left) ;

			//Copy the vertices
			result.setVertexCount(bezier.size());
			std::transform(
				bezier.cbegin(), bezier.cend(), klmCoords.values.cbegin(),
				result.getVertices().begin(),
				[reverse] (const position_vector_type& pos, const klm_vector_type& klm) -> VertexData {
					return VertexData(
						pos, 
						reverse ? klm_vector_type(-klm.x, -klm.y, klm.z) : klm
					);
				}
			);
			result.getVertices().front().type = VertexType::first;
			result.getVertices().back().type = VertexType::last;

			//Obtain the direction of the bezier and its control points
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

			//Check if the control points are protruding
			const bool b1IsProtruding = (fillSide==FillSide::left && !b1IsOutside) || 
										(fillSide==FillSide::right && b1IsOutside) ;
			const bool b2IsProtruding = (fillSide==FillSide::left && !b2IsOutside) || 
										(fillSide==FillSide::right && b2IsOutside) ;
			result.getVertices()[1].type = b1IsProtruding ? VertexType::controlProtruding : VertexType::control;
			result.getVertices()[2].type = b2IsProtruding ? VertexType::controlProtruding : VertexType::control;

			//Remove duplicated vertices
			if(	!approxZero(distance2(bezier[1], bezier[0])) ||
				!approxZero(distance2(bezier[1], bezier[3])) ) 
			{
				//First control point is dup
				std::swap(result.getVertices()[1], result.getVertices()[2]);
				std::swap(result.getVertices()[2], result.getVertices()[3]);
				result.setVertexCount(result.getVertexCount() - 1);
			} else if(	!approxZero(distance2(bezier[2], bezier[0])) ||
						!approxZero(distance2(bezier[2], bezier[1])) ||
						!approxZero(distance2(bezier[2], bezier[3])) ) 
			{
				//Second control point is dup
				std::swap(result.getVertices()[2], result.getVertices()[3]);
				result.setVertexCount(result.getVertexCount() - 1);
			}

			//Determine the layout of the vertices, so that although
			//the vertices are not going to be drawn in order, 
			//the triangle strip is well-formed
			if(result.getVertexCount() == 3) {
				//We will be forming a triangle
				//Establish the helper vertices
				if(result.getVertices()[1].type == VertexType::controlProtruding) {
					result.getVertices()[0].helperIndex = 2;
					result.getVertices()[1].helperIndex = 0;
				} else {
					result.getVertices()[0].helperIndex = 1;
				}

				//Add the indices to the result
				std::array<index_type, 3> indices;
				if(!(b1IsOutside || b2IsOutside)) {
					indices = {2, 1, 0};
				} else {
					indices = {0, 1, 2};
				}
				result.setIndices(indices);
			
			} else {
				//We will be forming a quad or a triangle with a vertex
				//in the middle
				assert(result.getVertexCount() == bezier.size());

				if(b1IsOutside == b2IsOutside) {
					//Both control points are on the same side. Suppose they are
					//outside, we'll correct it later if necessary
					//Determine if any of the control points overlaps the other
					if(isInsideTriangle(bezierDir, b1Dir, b2Dir)) {
						//b2 is overlapped by b1's triangle
						//Only b1 can be protruding
						result.getVertices()[2].type = VertexType::control;

						//In order to this triangulation to work b1 needs to be on top of b2
						if(!b1IsOutside) {
							std::swap(result.getVertices()[1], result.getVertices()[2]);

							//Establish the helper vertex (b2)
							assert(result.getVertices()[1].type == VertexType::control);
							result.getVertices()[0].helperIndex = 1;
							if(result.getVertices()[2].type == VertexType::controlProtruding) {
								result.getVertices()[2].helperIndex = 1;
							}
						} else {
							//Establish the helper vertex (b2)
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[0].helperIndex = 2;
							if(result.getVertices()[1].type == VertexType::controlProtruding) {
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
						//Only b2 can be protruding
						result.getVertices()[1].type = VertexType::control;

						//In order to this triangulation to work b2 needs to be on top of b1
						if(!b2IsOutside) {
							std::swap(result.getVertices()[1], result.getVertices()[2]);

							//Establish the helper vertex (b1)
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[0].helperIndex = 2;
							if(result.getVertices()[1].type == VertexType::controlProtruding) {
								result.getVertices()[1].helperIndex = 2;
							}
						} else {
							//Establish the helper vertex (b1)
							assert(result.getVertices()[1].type == VertexType::control);
							result.getVertices()[0].helperIndex = 1;
							if(result.getVertices()[2].type == VertexType::controlProtruding) {
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
						//Remove the possible self intersection
						if(b1Proj.x > b2Proj.x) {
							std::swap(result.getVertices()[1], result.getVertices()[2]);
						}

						//Establish the helper vertices
						if(result.getVertices()[1].type == VertexType::controlProtruding) {
							assert(result.getVertices()[2].type == VertexType::controlProtruding);
							result.getVertices()[0].helperIndex = 3; //Either 2 or 3
							result.getVertices()[1].helperIndex = 0; //Either 0 or 3
							result.getVertices()[2].helperIndex = 0; //Either 0 or 1
						} else {
							assert(result.getVertices()[1].type == VertexType::control);
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[0].helperIndex = 2; //Either 1 or 2
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
					//if necessary. Determine if any of the control points overlap the extremus
					//points.
					if(isInsideTriangle(b1Dir-bezierDir, b2Dir-bezierDir, -bezierDir)) {				
						//Origin is overlapped by the control points

						assert(false); //TODO test this case
						//Establish the helper vertices
						result.getVertices()[0].helperIndex = 3;
						if(result.getVertices()[1].type == VertexType::controlProtruding) {
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[1].helperIndex = 0;
						} else {
							assert(result.getVertices()[1].type == VertexType::control);
							assert(result.getVertices()[2].type == VertexType::controlProtruding);
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
						if(result.getVertices()[1].type == VertexType::controlProtruding) {
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[1].helperIndex = result.getVertices()[0].helperIndex;
						} else {
							assert(result.getVertices()[1].type == VertexType::control);
							assert(result.getVertices()[2].type == VertexType::controlProtruding);
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
						if(result.getVertices()[1].type == VertexType::controlProtruding) {
							assert(result.getVertices()[2].type == VertexType::control);
							result.getVertices()[0].helperIndex = 2; //Either 3 or 2
							result.getVertices()[1].helperIndex = 2; //Either 0 or 2
						} else {
							assert(result.getVertices()[1].type == VertexType::control);
							assert(result.getVertices()[2].type == VertexType::controlProtruding);
							result.getVertices()[0].helperIndex = 1; //Either 3 or 1
							result.getVertices()[2].helperIndex = 1; //Either 0 or 1
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
					//4 vertices, set complemetary values
					for(auto& index : result.getIndices()) {
						index = bezier.degree() - index;
					}
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

		}

		break;
	}
	default: { //DegeneratedCurveType::point
		//Ignore points
		break;
	}

	}

	//All vertices should be correctly classified
	for(const auto& vertex : result.getVertices()) {
		assert(vertex.type != VertexType::unknown);
	}

	return result;
}

}