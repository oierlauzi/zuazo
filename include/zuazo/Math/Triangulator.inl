#include "Triangulator.h"

#include "Geometry.h"
#include "../Exception.h"

#include <cassert>

namespace Zuazo::Math {

template<typename T, typename Index>
inline void Triangulator<T, Index>::operator()(	std::vector<index_type>& result,
												const polygon_type& polygon,
												index_type startIndex,
												index_type restartIndex ) const 
{
	//The following code is based on:
	//https://stackoverflow.com/questions/8980379/polygon-triangulation-into-triangle-strips-for-opengl-es

	//Line used to project the points. 
	//This will select the y component of the vectors when dot()
	constexpr vector_type L(0, 1); 

	//Ensure index type is big enough.
	if(polygon.size() + startIndex >= restartIndex) {
		throw Exception("Insufficent index size for triangulation");
	}

	//Check that the given polygon is simple (no intersecting edges)
	for(size_t i = 0; i < polygon.getSegmentCount(); ++i) {
		for(size_t j = i + 2; j < polygon.getSegmentCount()-1; ++j) {
			const auto& seg0 = polygon.getSegment(i);
			const auto& seg1 = polygon.getSegment(j);

			if(getIntersection(seg0, seg1)) {
				throw Exception("Unable to triangulate a complex polygon");
			}
		}
	}

	//Obtain the signed area of the polyon, as we'll
	//need it later
	const auto sArea = getSignedArea(polygon);

	//Initialize the index vector
	m_indices.resize(polygon.size());
	std::iota(m_indices.begin(), m_indices.end(), startIndex);

	while(!m_indices.empty()) {
		//Initialize the work vectors. This won't free their space
		m_monotonePolygonIndices.clear();

		//We only need to split it in a monotone polygon if it is not a triangle
		if(m_indices.size() > 3) {
			//Recreate the array of the sorted m_indices
			m_sortedIndices.resize(m_indices.size());
			std::iota(m_sortedIndices.begin(), m_sortedIndices.end(), 0);
			std::sort(
				m_sortedIndices.begin(), m_sortedIndices.end(),
				[this, &polygon, startIndex, &L] (Index aIdx, Index bIdx) -> bool {
					//Sort based on the y coordinate of the referenced vertex. Greatest first
					return 	dot(polygon.getPoint(this->m_indices[aIdx]-startIndex), L) > 
							dot(polygon.getPoint(this->m_indices[bIdx]-startIndex), L);
				}
			);

			//Create a monotone polygon from the m_indices
			for (size_t i = 0; i < m_sortedIndices.size(); ++i) {
				//Extract the vertices from the polygon
				const size_t nIth = m_sortedIndices[i];
				const size_t nNext = (nIth + 1) % m_indices.size();
				const size_t nPrev = (nIth + m_indices.size() - 1) % m_indices.size();
				const auto& ith = polygon.getPoint(m_indices[nIth]-startIndex);
				const auto& next = polygon.getPoint(m_indices[nNext]-startIndex);
				const auto& prev = polygon.getPoint(m_indices[nPrev]-startIndex);

				//Obtain the side on which they lie on
				const auto sidePrev = sign(dot(ith-prev, L));
				const auto sideNext = sign(dot(ith-next, L));

				if(sidePrev*sideNext >= 0) {
					//Both m_indices are on the same side. This means that we've
					//found a V shape (interior cusp), where ith is on the edge.
					//If ith is not the top nor the bottom, this means that this part is 
					//not monotone, so we need to split it up. For that, we'll
					//choose a diagonal that departs from nIth. We'll call to the
					//other point of the diagonal nSplit
					size_t nSplit = m_indices.size(); //Invalid index
					for(size_t iteration = 1; nSplit >= m_indices.size(); ++iteration) {
						//Get a candidate for splitting. For that, follow
						//the direction of the V. Remember that m_sortedIndices
						//is sorted top to bottom
						if(sidePrev + sideNext > 0) { //^
							if(i >= iteration) {
								nSplit = m_sortedIndices[i-iteration];
							} else {
								//We reached the top with no luck
								break;
							}
						} else { //V
							if(i+iteration < m_sortedIndices.size()) {
								nSplit = m_sortedIndices[i+iteration];
							} else {
								//We reached the bottom with no luck
								break;
							}
						}

						//Get the other vertex of the diagonal
						assert(nSplit < m_indices.size());
						const auto& split = polygon.getPoint(m_indices[nSplit]-startIndex);

						//Check that the diagonal's winding order is correct, 
						//as this will ensure that it is either completely
						//or partially inside the polygon
						const auto winding = getSignedArea(
							normalize(next-ith),
							normalize(split-ith),
							normalize(prev-ith)
						);

						if(sArea*winding <= 0) {
							//Windings missmatch. Line exits the polygon
							nSplit = m_indices.size();
							continue;
						}

						//Check if it intersects any edge other than itself. If not,
						//This means that the diagonal is completely inside
						const Zuazo::Math::Line<value_type, 2> divisorLine (ith, split);
						for(size_t j0 = 0; j0 < m_indices.size(); ++j0) {
							//Skip checks with neighbour
							const auto j1 = (j0+1) % m_indices.size();
							if(j0==nIth || j0==nSplit || j1==nIth || j1==nSplit){
								continue;
							}
							
							//Get the current test segment
							const Zuazo::Math::Line<value_type, 2> polygonSegment(
								polygon.getPoint(m_indices[j0]-startIndex), 
								polygon.getPoint(m_indices[j1]-startIndex)
							);

							//Check intersection
							if(Zuazo::Math::getIntersection(divisorLine, polygonSegment)) {
								//This segment exits the polygon. Try with another
								nSplit = m_indices.size();
								break;
							}
						}
					}

					//Extract a monotone polygon if the previous calculations were successful
					if(nSplit < m_indices.size()) {
						//Ensure ordering to step over all elements in between nBegin and nEnd
						const size_t nBegin = max(nSplit, nIth);
						const size_t nEnd = min(nSplit, nIth)+1;
						assert(nEnd <= nBegin);

						//Obtain the begin and end iterators for the edge range
						const auto begin = std::next(m_indices.cbegin(), nBegin);
						const auto end = std::next(m_indices.cbegin(), nEnd);

						//Add all the vertices in a circular manner. Remove all but the first and last
						assert(m_monotonePolygonIndices.empty());
						std::copy(begin, m_indices.cend(), std::back_inserter(m_monotonePolygonIndices));
						std::copy(m_indices.cbegin(), end, std::back_inserter(m_monotonePolygonIndices));

						//Discard all the edges but the first and last. Same manner as above
						m_indices.erase(std::next(begin), m_indices.cend());
						m_indices.erase(m_indices.cbegin(), std::prev(end));

						//We have finished creating a monotone polygon
						break;
					}
				}
			}
		}

		//The last step consists on dumping the rest of the m_indices
		if(m_monotonePolygonIndices.empty()) {
			m_monotonePolygonIndices = std::move(m_indices);
			assert(m_indices.empty());
		}
		assert(!m_monotonePolygonIndices.empty());

		//Ensure that at least a triangle was formed
		if(m_monotonePolygonIndices.size() >= 3) {
			//Begin populating the triangle strip m_indices
			if(!result.empty()) {
				//Unless this is the first triangle strip, restart with 
				//a fresh primitive
				result.push_back(restartIndex);
			}
			
			//Find the bottom-most vertex
			const auto topIte = std::min_element(
				m_monotonePolygonIndices.cbegin(), m_monotonePolygonIndices.cend(),
				[&polygon, startIndex, &L] (Index aIdx, Index bIdx) -> bool {
					return 	dot(polygon.getPoint(aIdx-startIndex), L) < 
							dot(polygon.getPoint(bIdx-startIndex), L);
				}
			);
			const size_t nTop = std::distance(m_monotonePolygonIndices.cbegin(), topIte);

			//Fill the index buffer with alternating sides, top to bottom,
			// so that a triangle strip is formed
			for(size_t i = 1; i <= m_monotonePolygonIndices.size(); ++i) {
				const size_t offset = i%2 ? m_monotonePolygonIndices.size() - i/2 : i/2;
				const size_t idx = (nTop + offset) % m_monotonePolygonIndices.size();
				result.push_back(m_monotonePolygonIndices[idx]);
			}
		}
	}
}

template<typename T, typename Index>
inline std::vector<typename Triangulator<T, Index>::index_type> 		
Triangulator<T, Index>::operator()(	const polygon_type& polygon,
									index_type startIndex,
									index_type restartIndex ) const
{
	std::vector<typename Triangulator<T, Index>::index_type> result;
	(*this)(result, polygon, startIndex, restartIndex);
	return result;
}


template<typename T, typename Index>
constexpr std::array<typename Triangulator<T, Index>::index_type, 4> 
Triangulator<T, Index>::triangulateQuad(const std::array<vector_type, 4>& quad,
										index_type startIndex ) noexcept
{
	//It must be convex and simple:
	assert(isConvex(quad[0], quad[1], quad[2], quad[3]));
	assert(
		!getIntersection(
			typename polygon_type::bezier_type(quad[0], quad[1]), 
			typename polygon_type::bezier_type(quad[2], quad[3])
		) 
	);
	assert(
		!getIntersection(
			typename polygon_type::bezier_type(quad[1], quad[2]), 
			typename polygon_type::bezier_type(quad[3], quad[0])
		) 
	);

	std::array<typename Triangulator<T, Index>::index_type, 4> result;

	//Obtain the diagonals
	const auto diagonal0 = typename polygon_type::bezier_type(quad[0], quad[2]);
	const auto diagonal1 = typename polygon_type::bezier_type(quad[1], quad[3]);

	//Split by the shortest diagonal. Length2 is used as it
	//is cheaper
	const auto lengthD0 = length2(diagonal0.getDelta());
	const auto lengthD1 = length2(diagonal1.getDelta());
	const size_t splitIndex = lengthD0 < lengthD1 ? 1 : 0;

	//Write the values
	constexpr std::array<index_type, result.size()> offsets = { 0, 1, 3, 2 };
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = startIndex + (splitIndex+offsets[i])%quad.size();
	}

	return result;
}

}