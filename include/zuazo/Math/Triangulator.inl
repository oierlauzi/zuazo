#include "Triangulator.h"

#include "Geometry.h"
#include "Combinatorics.h"
#include "../Exception.h"

#include <cassert>
#include <limits>

namespace Zuazo::Math {

template<typename T, typename Index>
constexpr Triangulator<T, Index>::DPState::DPState(	bool visible, 
													value_type weight, 
													index_type bestVertex ) noexcept
	: visible(visible)
	, weight(weight)
	, bestVertex(bestVertex)
{

}



template<typename T, typename Index>
inline void Triangulator<T, Index>::operator()(	std::vector<index_type>& result,
												const polygon_type& polygon,
												index_type startIndex,
												index_type restartIndex ) const 
{
	if(polygon.size() == 3) {
		//Triangulating a triangle :-)
		//Restart a new triangle and add 3 consecutive vertices
		if(!result.empty()) {
			result.emplace_back(restartIndex);
		}

		const std::array<index_type, 3> triangleIndices = {
			startIndex + 0,
			startIndex + 1,
			startIndex + 2
		};

		result.insert(
			result.cend(), 
			triangleIndices.cbegin(), 
			triangleIndices.cend()
		);
	} else if(polygon.size() == 4 && isConvex(polygon)) {
		//Triangulate a quad
		//Restart a new triangle and add 4 alternating vertices
		if(!result.empty()) {
			result.emplace_back(restartIndex);
		}
		
		const auto quadIndices = triangulateQuad(
			polygon.getPoint(0),
			polygon.getPoint(1),
			polygon.getPoint(2),
			polygon.getPoint(3),
			startIndex
		);

		result.insert(
			result.cend(), 
			quadIndices.cbegin(), 
			quadIndices.cend()
		);

	} else if(polygon.size() > 3) {
		//General case

		//The following code is based on:
		//https://github.com/ivanfratric/polypartition/blob/master/src/polypartition.cpp

		const auto n = polygon.size();
		const auto dpStateRows = n-1;

		//Dynamic programing state gets stored in a 
		//triangle-like structure, so we create an array
		//to accommodate such data. Initialize it with the
		//default values
		std::fill(m_dpState.begin(), m_dpState.end(), DPState());
		m_dpState.resize(triangularNumber(dpStateRows), DPState());

		//Initialize the visibility checks
		for(size_t i = 0; i < n-1; ++i) {
			const size_t im1 = i > 0 ? i-1 : n-1; //If i is 0, roll back to the last value
			const size_t ip1 = i+1; //Next i. At most n-1, as this value is not reached by i
			assert(im1 < n);
			assert(ip1 < n);

			//Extract the first point from the polygon
			const auto& p0 = polygon.getPoint(i);

			for(size_t j = i+2; j < n; ++j) {
				const size_t jm1 = j-1; //j will always be > 0
				const size_t jp1 = j < n-1 ? j+1 : 0; //Next j
				assert(jm1 < n);
				assert(jp1 < n);

				const size_t dpStateIndex = triangularNumber(j-1)+i;
				assert(dpStateIndex < m_dpState.size());
				auto& dpState = m_dpState[dpStateIndex];

				//Extract the second point from the polygon
				const auto& p1 = polygon.getPoint(j);

				//Perform the first visibility check
				{
					//Extract the third and fourth points from the polygon
					const auto& p2 = polygon.getPoint(im1);
					const auto& p3 = polygon.getPoint(ip1);

					//Check if p1 is inside the angle formed
					//by p0-p2-p3
					if(!isInCone(p2, p0, p3, p1)) {
						//This is not inside the angle
						dpState.visible = false;
						continue;
					}
				}

				//Perform the second visibility check
				{
					//Extract the third and fourth points from the polygon
					const auto& p2 = polygon.getPoint(jm1);
					const auto& p3 = polygon.getPoint(jp1);

					//Check if p0 is inside the angle formed
					//by p1-p2-p3
					if(!isInCone(p2, p1, p3, p0)) {
						//This is not inside the angle
						dpState.visible = false;
						continue;
					}
				}

				//Intersection checks
				for(size_t k = 0; k < n; ++k) {
					size_t kp1 = k < n-1 ? k+1 : 0;

					//Skip checking intersections with neighbors
					if(k == i || k == j || kp1 == i || kp1 ==j) {
						continue;
					}

					const auto& p2 = polygon.getPoint(k);
					const auto& p3 = polygon.getPoint(kp1);

					const Line<value_type, 2> ln0(p0, p1);
					const Line<value_type, 2> ln1(p2, p3);

					if(getIntersection(ln0, ln1)) {
						//Lines intersect
						dpState.visible = false;
						break;
					}
				}

				if(!dpState.visible){
					continue;
				}

				//At this point we can ensure that the point is visible for this vertex
				assert(dpState.visible == true);
			}
		}

		//Initialize first value of the last row of 
		//the dynamic states
		m_dpState[m_dpState.size() - dpStateRows] = DPState();

		//Calculate the weights of each vertex
		for(size_t gap = 2; gap < n; ++gap) {
			for(size_t i = 0; i < n-gap; ++i) {
				const size_t j = i+gap;

				const size_t dpStateIndex = triangularNumber(j-1)+i;
				assert(dpStateIndex < m_dpState.size());
				auto& dpState = m_dpState[dpStateIndex];

				//Vertex must be visible
				if(!dpState.visible) {
					continue;
				}

				index_type bestVertex = ~index_type(); //Invalid index
				value_type minWeight = std::numeric_limits<value_type>::max();

				for(size_t k = i+1; k < j; ++k) {
					//Check visibility of one of the other vertices
					const size_t dpStateIndexKI = triangularNumber(k-1)+i;
					assert(dpStateIndexKI < m_dpState.size());
					const auto& dpStateKI = m_dpState[dpStateIndexKI];
					if(!dpStateKI.visible) {
						continue;
					}

					//Check visibility of one of the other vertices
					const size_t dpStateIndexJK = triangularNumber(j-1)+k;
					assert(dpStateIndexJK < m_dpState.size());
					const auto& dpStateJK = m_dpState[dpStateIndexJK];
					if(!dpStateJK.visible) {
						continue;
					}

					//Get the distances between the vertices. //Using distance2 as it is cheaper
					const auto d1 = (k < i+2) ? 0 : distance2(polygon.getPoint(k), polygon.getPoint(i));
					const auto d2 = (j < k+2) ? 0 : distance2(polygon.getPoint(j), polygon.getPoint(k));

					//Calculate the new weight
					const auto weight = dpStateKI.weight + dpStateJK.weight + d1 + d2;

					//If the weight is lower than the minimum, set it as the best candidate
					//In the first iteration, miWeight will be set to inf, so this will stablish
					//the weight for the first time
					if(weight < minWeight) {
						bestVertex = k;
						minWeight = weight;
					}
				}

				if(bestVertex >= n) {
					//A error happened findinding an appropiate vertex
					throw Exception("Error gathering best vertices for triangulating the polygon");
				}

				//Write the new data to the current dynamic programming state
				dpState.bestVertex = bestVertex;
				dpState.weight = minWeight;
			}
		}

		enum class StrippingSide {
			NONE,
			LEFT,
			RIGHT,
		};

		//Start adding diagonals. Also connect the
		//last 2 edges
		m_diagonals.clear();
		m_diagonals.emplace_back(0, n-1);
		StrippingSide strippingSide = StrippingSide::NONE;

		while(!m_diagonals.empty()) {
			const auto diagonal = m_diagonals.front();
			m_diagonals.pop_front();

			//Obtain the best vertex for triangulation for this diagonal
			const size_t dpStateIndex = triangularNumber(diagonal.second-1) + diagonal.first;
			assert(dpStateIndex < m_dpState.size());
			const auto& dpState = m_dpState[dpStateIndex];
			const auto bestVertex = dpState.bestVertex;

			if(bestVertex >= n) {
				//Error happened
				throw Exception("Error triangulating the polygon");
			}

			//Check if we can use stripping
			if(strippingSide != StrippingSide::NONE) {
				//We only need to push 1 index, as the previous
				//2 are part of the previous triangle. Push
				//the last index of the new triangle
				result.emplace_back(bestVertex + startIndex);
			} else {
				//Restart the primitive
				if(!result.empty()) {
					result.emplace_back(restartIndex);
				}

				//Push 3 vertices to form a tri
				const std::array<index_type, 3> triangleVertices = {
					diagonal.second + startIndex,
					diagonal.first + startIndex,
					bestVertex + startIndex
				};
				result.insert(result.cend(), triangleVertices.cbegin(), triangleVertices.cend());

				strippingSide = StrippingSide::RIGHT;
			}

			//Segment for the next iteration. Stripe in alternating sides
			if(strippingSide == StrippingSide::RIGHT) {
				if(bestVertex > diagonal.first+1) {
					//Only if this diagonal is on the front
					//triangle can be stripped
					strippingSide = m_diagonals.empty() ? StrippingSide::LEFT : StrippingSide::NONE;

					m_diagonals.emplace_back(diagonal.first, bestVertex);
				} else {
					strippingSide = StrippingSide::NONE;
				}

				if(diagonal.second > bestVertex+1) {
					m_diagonals.emplace_back(bestVertex, diagonal.second);
				}

			} else { assert(strippingSide == StrippingSide::LEFT);
				if(diagonal.second > bestVertex+1) {
					//Only if this diagonal is on the front
					//triangle can be stripped
					strippingSide = m_diagonals.empty() ? StrippingSide::RIGHT : StrippingSide::NONE;

					m_diagonals.emplace_back(bestVertex, diagonal.second);
				} else {
					strippingSide = StrippingSide::NONE;
				}

				if(bestVertex > diagonal.first+1) {
					m_diagonals.emplace_back(diagonal.first, bestVertex);
				}

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
Triangulator<T, Index>::triangulateQuad(const vector_type& v0,
										const vector_type& v1,
										const vector_type& v2,
										const vector_type& v3,
										index_type startIndex ) noexcept
{
	//It must be convex and simple. In quads, one implies the other
	assert(isConvex(v0, v1, v2, v3));

	std::array<typename Triangulator<T, Index>::index_type, 4> result;

	//Obtain the diagonals
	const auto diagonal0 = typename polygon_type::bezier_type(v0, v2);
	const auto diagonal1 = typename polygon_type::bezier_type(v1, v3);

	//Split by the shortest diagonal. Length2 is used as it
	//is cheaper
	const auto lengthD0 = length2(diagonal0.getDelta());
	const auto lengthD1 = length2(diagonal1.getDelta());
	const size_t splitIndex = lengthD0 < lengthD1 ? 1 : 0;

	//Write the values
	constexpr std::array<index_type, result.size()> offsets = { 0, 1, 3, 2 };
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = startIndex + (splitIndex+offsets[i]) % offsets.size();
	}

	return result;
}



template<typename T, typename Index>
constexpr bool Triangulator<T, Index>::isBelow(const vector_type& delta) noexcept {
	return (delta.y < 0) || ((delta.y == 0) && (delta.x > 0));
}

template<typename T, typename Index>
constexpr bool Triangulator<T, Index>::isAbove(const vector_type& delta) noexcept {
	return (delta.y > 0) || ((delta.y == 0) && (delta.x < 0));
}

}