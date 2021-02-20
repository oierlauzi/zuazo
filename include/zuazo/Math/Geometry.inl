#include "Geometry.h"

#include "Absolute.h"

#include <type_traits>
#include <algorithm>
#include <cmath>

namespace Zuazo::Math {

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(const Vec2<T> vec) noexcept {
	const auto len = length(vec);
	const auto c = vec.x / len; //cos: x
	const auto s = vec.y / len; //sin: y

	//Note that the rotation matrix is defined as the following:
	// [cos(O), -sin(O); 
	//  sin(O), cos(O)]
	// As we want to rotate in the opposite direction of the direction
	//vector, substituting O by -O inverts the sign of the sines.
	return Mat2x2<typename Vec2<T>::value_type>(
		 c, s,
		-s, c
	);
}


template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Line<T, 2>& line,
																		const typename Line<T, 2>::value_type& point ) noexcept
{
	const auto delta = point - line.front();
	const auto direction = normalize(line.back() - line.front());
	const auto normal = typename Line<T, 2>::value_type(-direction.y, direction.x);
	return dot(normal, delta);
}


template<typename T, typename Index>
inline std::vector<Index> triangulate(Utils::BufferView<const Vec2<T>> polygon) {
	static_assert(std::is_integral<Index>::value && std::is_unsigned<Index>::value , "Index must be a unsigned integer");
	constexpr auto PRIMITIVE_RESTART_INDEX = ~Index();
	constexpr Vec2<T> L(0, 1); //Line used to project the points. This will select the y component of the vectors when dot()

	//Ensure index type is big enough. < is used as the last value is reserved
	//for restarting the primitive
	assert(polygon.size() < std::numeric_limits<Index>::max());

	//The following code is based on:
	//https://stackoverflow.com/questions/8980379/polygon-triangulation-into-triangle-strips-for-opengl-es
	std::vector<Index> result;

	//Initialize the index vector
	std::vector<Index> indices(polygon.size());
	std::iota(indices.begin(), indices.end(), 0);

	std::vector<Index> sortedIndices, removeIndices, monotonePolygonIndices;
	while(!indices.empty()) {
		//Recreate the array of the sorted indices
		sortedIndices.resize(indices.size());
		std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
		std::sort(
			sortedIndices.begin(), sortedIndices.end(),
			[&indices, &polygon, &L] (Index aIdx, Index bIdx) -> bool {
				//Sort based on the y coordinate of the referenced vertex. Greatest first
				return dot(polygon[indices[aIdx]], L) > dot(polygon[indices[bIdx]], L);
			}
		);

		//Initialize the work vectors. This won't free their space
		removeIndices.clear();
		monotonePolygonIndices.clear();

		//Create a monotone polygon from the indices
		for (size_t i = 0; i < sortedIndices.size(); ++i) {
			assert(removeIndices.empty());
			assert(monotonePolygonIndices.empty());

			//Extract the vertices from the polygon
			const size_t nIth = sortedIndices[i];
			const size_t nNext = (nIth + 1) % indices.size();
			const size_t nPrev = (nIth + indices.size() - 1) % indices.size();
			const auto& ith = polygon[indices[nIth]];
			const auto& next = polygon[indices[nNext]];
			const auto& prev = polygon[indices[nPrev]];

			//Obtain the side on which they lie on
			const auto sidePrev = sign(dot(ith-prev, L));
        	const auto sideNext = sign(dot(ith-next, L));

			if(sidePrev*sideNext >= 0) {
				//Both indices are on the same side.
				//Check that the line does not exit the polygon in case
				//of a concave edge
				size_t nBegin = indices.size(); //Invalid index
				if(sidePrev + sideNext > 0) {
					if(i > 0) {
						nBegin = sortedIndices[i-1];
					}
				} else {
					if(i+1 < sortedIndices.size()) {
						nBegin = sortedIndices[i+1];
					}
				}

				if(nBegin < indices.size()) {
					//Ensure ordering to step over all elements
					const size_t nEnd = min(nBegin, nIth);
					nBegin = max(nBegin, nIth);
					assert(nEnd < nBegin);

					//Add all the vertices. Remove all but the first and last
					monotonePolygonIndices.push_back(indices[nBegin]);
					for(size_t j = (nBegin+1) % indices.size(); j != nEnd; j = (j+1) % indices.size()) {
						monotonePolygonIndices.push_back(indices[j]);
						removeIndices.push_back(j);
					}
					monotonePolygonIndices.push_back(indices[nEnd]);

					//Discard all unwanted indices for the next iteration
					//Sort is used in order to remove backwards, so that lower indices remain valid
					std::sort(removeIndices.begin(), removeIndices.end());
					for(auto ite = removeIndices.crbegin(); ite != removeIndices.crend(); ++ite) {
						assert(*ite < indices.size());
						indices.erase(std::next(indices.cbegin(), *ite));
					}

					//We have finished creating a monotone polygon
					break;
				}
			}
		}

		//The last step consists on dumping the rest of the indices
		if(monotonePolygonIndices.empty()) {
			monotonePolygonIndices = std::move(indices);
			assert(indices.empty());
		}
		assert(!monotonePolygonIndices.empty());

		if(monotonePolygonIndices.size() >= 3) {
			//Begin populating the triangle strip indices
			if(!result.empty()) {
				//Unless this is the first triangle strip, restart with 
				//a fresh primitive
				result.push_back(PRIMITIVE_RESTART_INDEX);
			}
			
			//Find the bottom-most vertex
			const auto topIte = std::min_element(
				monotonePolygonIndices.cbegin(), monotonePolygonIndices.cend(),
				[&indices, &polygon, &L] (Index aIdx, Index bIdx) -> bool {
					return dot(polygon[aIdx], L) < dot(polygon[bIdx], L);
				}
			);
			const size_t nTop = std::distance(monotonePolygonIndices.cbegin(), topIte);

			//Fill the index buffer with alternating sides, top to bottom,
			// so that a triangle strip is formed
			for(size_t i = 1; i <= monotonePolygonIndices.size(); ++i) {
				const size_t offset = i%2 ? monotonePolygonIndices.size() - i/2 : i/2;
				const size_t idx = (nTop + offset) % monotonePolygonIndices.size();
				result.push_back(monotonePolygonIndices[idx]);
			}
		}
	}

	return result;
}


}