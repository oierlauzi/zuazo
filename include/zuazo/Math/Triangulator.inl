#include "Triangulator.h"

#include "Geometry.h"
#include "../Exception.h"

namespace Zuazo::Math {

template<typename T, typename Index>
inline void Triangulator<T, Index>::operator()(	std::vector<index_type>& result,
												Utils::BufferView<const vector_type> polygon,
												index_type restartIndex ) const 
{
	//The following code is based on:
	//https://stackoverflow.com/questions/8980379/polygon-triangulation-into-triangle-strips-for-opengl-es

	//Line used to project the points. 
	//This will select the y component of the vectors when dot()
	constexpr vector_type L(0, 1); 

	//Ensure index type is big enough.
	if(polygon.size() >= restartIndex) {
		throw Exception("Insufficent index size for triangulation");
	}

	//Check that the given polygon is simple (no intersecting edges)
	for(size_t i = 1; i < polygon.size(); ++i) {
		for(size_t j = i + 2; j < polygon.size(); ++j) {
			const Line<value_type, 2> seg0(polygon[i-1], polygon[i]);
			const Line<value_type, 2> seg1(polygon[j-1], polygon[j]);

			if(getIntersection(seg0, seg1)) {
				throw Exception("Unable to triangulate a complex polygon");
			}
		}
	}

	//Initialize the index vector
	m_indices.resize(polygon.size());
	std::iota(m_indices.begin(), m_indices.end(), 0);

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
				[this, &polygon, &L] (Index aIdx, Index bIdx) -> bool {
					//Sort based on the y coordinate of the referenced vertex. Greatest first
					return dot(polygon[this->m_indices[aIdx]], L) > dot(polygon[this->m_indices[bIdx]], L);
				}
			);

			//Create a monotone polygon from the m_indices
			for (size_t i = 0; i < m_sortedIndices.size(); ++i) {
				assert(m_monotonePolygonIndices.empty());

				//Extract the vertices from the polygon
				const size_t nIth = m_sortedIndices[i];
				const size_t nNext = (nIth + 1) % m_indices.size();
				const size_t nPrev = (nIth + m_indices.size() - 1) % m_indices.size();
				const auto& ith = polygon[m_indices[nIth]];
				const auto& next = polygon[m_indices[nNext]];
				const auto& prev = polygon[m_indices[nPrev]];

				//Obtain the side on which they lie on
				const auto sidePrev = sign(dot(ith-prev, L));
				const auto sideNext = sign(dot(ith-next, L));

				if(sidePrev*sideNext >= 0) {
					//Both m_indices are on the same side.
					//Decide the direction in which we are going to 
					//travel across the edges
					size_t nBegin = m_indices.size(); //Invalid index
					if(sidePrev + sideNext > 0) {
						if(i > 0) {
							nBegin = m_sortedIndices[i-1];
						}
					} else {
						if(i+1 < m_sortedIndices.size()) {
							nBegin = m_sortedIndices[i+1];
						}
					}

					//Check that the line does not exit the polygon
					if(nBegin < m_indices.size()) {
						//Get the line which divides the polygon in 2
						const Zuazo::Math::Line<value_type, 2> divisor(
							polygon[m_indices[nIth]], 
							polygon[m_indices[nBegin]]
						);

						//Check if it intersects any edge other than itself
						for(size_t j0 = 0; j0 < m_indices.size(); ++j0) { 
							const auto j1 = (j0+1) % m_indices.size();
							if(j0==nIth || j0==nBegin || j1==nIth || j1==nBegin){
								continue;
							}
							
							//Get the current test segment
							const Zuazo::Math::Line<value_type, 2> polygonSegment(
								polygon[m_indices[j0]], 
								polygon[m_indices[j1]]
							);

							//Check intersection
							if(Zuazo::Math::getIntersection(divisor, polygonSegment)) {
								//This segment exits the polygon. Try with another
								nBegin = m_indices.size();
								break;
							}
						}
					}

					//Extract a monotone polygon if the previous calculations were successful
					if(nBegin < m_indices.size()) {
						//Ensure ordering to step over all elements in between nBegin and nEnd
						const size_t nEnd = min(nBegin, nIth)+1;
						nBegin = max(nBegin, nIth);
						assert(nEnd <= nBegin);

						//Obtain the begin and end iterators for the edge range
						const auto begin = std::next(m_indices.cbegin(), nBegin);
						const auto end = std::next(m_indices.cbegin(), nEnd);

						//Add all the vertices in a circular manner. Remove all but the first and last
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
				[&polygon, &L] (Index aIdx, Index bIdx) -> bool {
					return dot(polygon[aIdx], L) < dot(polygon[bIdx], L);
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
Triangulator<T, Index>::operator()(	Utils::BufferView<const vector_type> polygon,
									index_type restartIndex ) const
{
	std::vector<typename Triangulator<T, Index>::index_type> result;
	(*this)(result, polygon, restartIndex);
	return result;
}

}