#include "Triangulator.h"

#include "Geometry.h"
#include "Combinatorics.h"
#include "../Exception.h"

#include <cassert>
#include <limits>

namespace Zuazo::Math {

template<typename T, typename Index>
constexpr Triangulator<T, Index>::Visibility::Visibility(	bool visible, 
															value_type weight, 
															index_type bestVertex ) noexcept
	: visible(visible)
	, weight(weight)
	, bestVertex(bestVertex)
{
}



template<typename T, typename Index>
constexpr Triangulator<T, Index>::TriangleListGenerator::TriangleListGenerator(	std::vector<index_type>& result,
																				index_type startIndex ) noexcept
	: m_result(result)
	, m_startIndex(startIndex)
{
}

template<typename T, typename Index>
inline TriangleSideFlags
Triangulator<T, Index>::TriangleListGenerator::operator()(	const Diagonal& diagonal, 
															index_type bestVertex,
															TriangleSideFlags,
															size_t,
															bool )
{
	auto& result = m_result.get();

	//Insert 3 new vertices at the back
	const std::array<index_type, 3> triangle = {
		diagonal.first + m_startIndex,
		bestVertex + m_startIndex,
		diagonal.second + m_startIndex
	};

	result.insert(result.cend(), triangle.cbegin(), triangle.cend());

	return TriangleSideFlags::NONE; //We do not care about the next side
}



template<typename T, typename Index>
constexpr Triangulator<T, Index>::TriangleStripGenerator::TriangleStripGenerator(	std::vector<index_type>& result,
																					index_type startIndex,
																					index_type restartIndex ) noexcept
	: m_result(result)
	, m_startIndex(startIndex)
	, m_restartIndex(restartIndex)
	, m_stripingSide(TriangleSideFlags::NONE)
{
}

template<typename T, typename Index>
inline TriangleSideFlags
Triangulator<T, Index>::TriangleStripGenerator::operator()(	const Diagonal& diagonal, 
															index_type bestVertex,
															TriangleSideFlags adjointDiagonals,
															size_t queueLength,
															bool )
{
	auto& result = m_result.get();

	//Check if we can use tri-striping
	if(m_stripingSide != TriangleSideFlags::NONE) {
		//We only need to push 1 index, as the previous
		//2 are part of the previous triangle. Push
		//the last index of the new triangle
		result.emplace_back(bestVertex + m_startIndex);
	} else {
		//Restart the primitive, as we can not form a triangle strip
		if(!result.empty()) {
			result.emplace_back(m_restartIndex);
		}

		//Push 3 vertices to form a tri
		const std::array<index_type, 3> triangle = {
			diagonal.second + m_startIndex,
			diagonal.first + m_startIndex,
			bestVertex + m_startIndex
		};
		result.insert(result.cend(), triangle.cbegin(), triangle.cend());

		//We have arbitrarily chosen to do so
		m_stripingSide = TriangleSideFlags::LEFT;
	}

	//Segment for the next iteration. Stripe in alternating sides
	if(m_stripingSide == TriangleSideFlags::LEFT) {
		if((adjointDiagonals & TriangleSideFlags::RIGHT) != TriangleSideFlags::NONE) {
			//Only if this diagonal is on the front
			//triangle can be stripped
			m_stripingSide = queueLength == 0 ? TriangleSideFlags::RIGHT : TriangleSideFlags::NONE;
		} else {
			m_stripingSide = TriangleSideFlags::NONE;
		}

	} else { assert(m_stripingSide == TriangleSideFlags::RIGHT);
		if((adjointDiagonals & TriangleSideFlags::LEFT) != TriangleSideFlags::NONE) {
			//Only if this diagonal is on the front
			//triangle can be stripped
			m_stripingSide = queueLength == 0 ? TriangleSideFlags::LEFT : TriangleSideFlags::NONE;
		} else {
			m_stripingSide = TriangleSideFlags::NONE;
		}
	}

	return m_stripingSide; //Next in the queue should be the side we're striping
}



template<typename T, typename Index>
template<typename F>
inline void Triangulator<T, Index>::operator()(	const polygon_type& polygon,
												F&& triangleCallback ) const
{
	if(polygon.size() == 3) {
		//Trivial case
		std::forward<F>(triangleCallback)(
			Diagonal(0, 2), 			//First and last vertices
			1,							//The only remaining vertex
			TriangleSideFlags::NONE,
			0,
			true
		);
	} else if(polygon.size() == 4 && isConvex(polygon)) {
		//Quad triangulation can be optimized
		const auto indices = triangulateQuad(
			triangleList,
			polygon.getPoint(0),
			polygon.getPoint(1),
			polygon.getPoint(2),
			polygon.getPoint(3),
			0
		);

		//In order to have the next triangle on the left:
		assert(indices[2] == indices[3]);
		assert(indices[1] == indices[4]);

		//First triangle
		std::forward<F>(triangleCallback)(
			Diagonal(indices[1], indices[0]),
			indices[2],
			TriangleSideFlags::RIGHT,
			0,
			true
		);

		//Second triangle
		std::forward<F>(triangleCallback)(
			Diagonal(indices[4], indices[3]),
			indices[5],
			TriangleSideFlags::NONE,
			0,
			false
		);

	} else if(polygon.size() > 3) {
		//The following code is based on:
		//https://github.com/ivanfratric/polypartition/blob/master/src/polypartition.cpp
		const auto n = polygon.size();
		const auto visibilityRows = n-1;

		//Dynamic programing state gets stored in a 
		//triangle-like structure, so we create an array
		//to accommodate such data. Initialize it with the
		//default values
		assert(m_visibilities.empty());
		m_visibilities.resize(triangularNumber(visibilityRows), Visibility());

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

				const size_t visibilityIndex = triangularNumber(j-1)+i;
				assert(visibilityIndex < m_visibilities.size());
				auto& visibility = m_visibilities[visibilityIndex];

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
						visibility.visible = false;
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
						visibility.visible = false;
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
						visibility.visible = false;
						break;
					}
				}

				if(!visibility.visible){
					continue;
				}

				//At this point we can ensure that the point is visible for this vertex
				assert(visibility.visible == true);
			}
		}

		//Initialize first value of the last row of 
		//the dynamic states
		m_visibilities[m_visibilities.size() - visibilityRows] = Visibility();

		//Calculate the weights of each vertex
		for(size_t gap = 2; gap < n; ++gap) {
			for(size_t i = 0; i < n-gap; ++i) {
				const size_t j = i+gap;

				const size_t visibilityIndex = triangularNumber(j-1)+i;
				assert(visibilityIndex < m_visibilities.size());
				auto& visibility = m_visibilities[visibilityIndex];

				//Vertex must be visible
				if(!visibility.visible) {
					continue;
				}

				index_type bestVertex = ~index_type(); //Invalid index
				value_type minWeight = std::numeric_limits<value_type>::max();

				for(size_t k = i+1; k < j; ++k) {
					//Check visibility of one of the other vertices
					const size_t visibilityIndexKI = triangularNumber(k-1)+i;
					assert(visibilityIndexKI < m_visibilities.size());
					const auto& visibilityKI = m_visibilities[visibilityIndexKI];
					if(!visibilityKI.visible) {
						continue;
					}

					//Check visibility of one of the other vertices
					const size_t visibilityIndexJK = triangularNumber(j-1)+k;
					assert(visibilityIndexJK < m_visibilities.size());
					const auto& visibilityJK = m_visibilities[visibilityIndexJK];
					if(!visibilityJK.visible) {
						continue;
					}

					//Get the distances between the vertices. //Using distance2 as it is cheaper
					const auto d1 = (k < i+2) ? 0 : distance2(polygon.getPoint(k), polygon.getPoint(i));
					const auto d2 = (j < k+2) ? 0 : distance2(polygon.getPoint(j), polygon.getPoint(k));

					//Calculate the new weight
					const auto weight = visibilityKI.weight + visibilityJK.weight + d1 + d2;

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
				visibility.bestVertex = bestVertex;
				visibility.weight = minWeight;
			}
		}


		//Connect the last 2 edges to begin with
		assert(m_diagonals.empty());
		m_diagonals.emplace_back(0, polygon.size()-1);
		bool isFalseDiagonal = true;

		while(!m_diagonals.empty()) {
			const auto& diagonal = m_diagonals.front();

			//Obtain the best vertex for triangulation for this diagonal
			const size_t visibilityIndex = triangularNumber(diagonal.second-1) + diagonal.first;
			assert(visibilityIndex < m_visibilities.size());
			const auto& visibility = m_visibilities[visibilityIndex];

			//Check that it has been filled
			if(visibility.bestVertex >= polygon.size()) {
				//Error happened
				throw Exception("Error triangulating the polygon");
			}

			//		 bestVertex
			//           X
			//left side / \ right side
			//         /   \
			// second X-----X first

			//Check which sides are available for this triangle
			auto nextDiagonals = TriangleSideFlags::NONE;
			if(visibility.bestVertex > diagonal.first+1) {
				nextDiagonals |= TriangleSideFlags::RIGHT;
			}
			if(diagonal.second > visibility.bestVertex+1) {
				nextDiagonals |= TriangleSideFlags::LEFT;
			}

			//Call the given function with the available parameters
			const auto first = std::forward<F>(triangleCallback)(
				const_cast<const Diagonal&>(diagonal),
				const_cast<const index_type&>(visibility.bestVertex),
				const_cast<const TriangleSideFlags&>(nextDiagonals),
				m_diagonals.size()-1,
				isFalseDiagonal
			);
			isFalseDiagonal = false; //Only the first one will be fake

			//Add the new diagonals in the desired order
			if((first & TriangleSideFlags::LEFT) != TriangleSideFlags::NONE) {
				//Left first
				if((nextDiagonals & TriangleSideFlags::LEFT) != TriangleSideFlags::NONE) {
					m_diagonals.emplace_back(visibility.bestVertex, diagonal.second);
				}

				if((nextDiagonals & TriangleSideFlags::RIGHT) != TriangleSideFlags::NONE){
					m_diagonals.emplace_back(diagonal.first, visibility.bestVertex);
				}

			} else {
				//Right first
				if((nextDiagonals & TriangleSideFlags::RIGHT) != TriangleSideFlags::NONE) {
					m_diagonals.emplace_back(diagonal.first, visibility.bestVertex);
				}

				if((nextDiagonals & TriangleSideFlags::LEFT) != TriangleSideFlags::NONE) {
					m_diagonals.emplace_back(visibility.bestVertex, diagonal.second);
				}

			}

			//We're done with this diagonal, continue to the next one
			m_diagonals.pop_front();
		}

		//Clear stuff for the next iteration
		m_visibilities.clear();
		assert(m_diagonals.empty()); //This is the exit contition, so should be already clear
	}
}



template<typename T, typename Index>
constexpr std::array<typename Triangulator<T, Index>::index_type, 6> 
Triangulator<T, Index>::triangulateQuad(TriangleListTag,
										const vector_type& v0,
										const vector_type& v1,
										const vector_type& v2,
										const vector_type& v3,
										index_type startIndex ) noexcept
{
	//It must be convex and simple. In quads, one implies the other
	assert(isConvex(v0, v1, v2, v3));

	std::array<typename Triangulator<T, Index>::index_type, 6> result;

	//Obtain the diagonals
	const auto diagonal0 = typename polygon_type::bezier_type(v0, v2);
	const auto diagonal1 = typename polygon_type::bezier_type(v1, v3);

	//Split by the shortest diagonal. Length2 is used as it
	//is cheaper
	const auto lengthD0 = length2(diagonal0.getDelta());
	const auto lengthD1 = length2(diagonal1.getDelta());
	const size_t splitIndex = lengthD0 < lengthD1 ? 1 : 0;

	//Write the values
	constexpr std::array<index_type, result.size()> offsets = { 0, 1, 3, 3, 1, 2 };
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = startIndex + (splitIndex+offsets[i]) % 4;
	}

	return result;
}

template<typename T, typename Index>
constexpr std::array<typename Triangulator<T, Index>::index_type, 4> 
Triangulator<T, Index>::triangulateQuad(TriangleStripTag,
										const vector_type& v0,
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
		result[i] = startIndex + (splitIndex+offsets[i]) % 4;
	}

	return result;
}

}