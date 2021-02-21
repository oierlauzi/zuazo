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

template<typename T, size_t N>
constexpr typename Vec<T, N>::value_type distance(const Vec<T, N>& a, const Vec<T, N>& b) noexcept {
	return length(b - a);
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

template<typename T>
constexpr bool getIntersection(	const Line<T, 2>& a,
								const Line<T, 2>& b,
								typename Line<T, 2>::value_type* point ) noexcept
{
	const auto dirA = a.back() - a.front();
	const auto dirB = b.back() - b.front();
	const auto deltaOrig = a.front() - b.front();

	const auto sNum = det(Mat2x2<T>(dirA, deltaOrig));
	const auto tNum = det(Mat2x2<T>(dirB, deltaOrig));
	const auto den = det(Mat2x2<T>(dirA, dirB));
	const auto s = sNum / den;
	const auto t = tNum / den;

	constexpr typename Line<T, 2>::value_type::value_type ZERO(0), ONE(1);
	const bool result = isInRange(s, ZERO, ONE) && isInRange(t, ZERO, ONE);
	if(result && point) {
		*point = a(s);
		assert(distance(*point, b(t)) < 1e-12);
	}

	return result;
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

	std::vector<Index> sortedIndices, monotonePolygonIndices;
	while(!indices.empty()) {
		//Initialize the work vectors. This won't free their space
		monotonePolygonIndices.clear();

		//We only need to split it in a monotone polygon if it is not a triangle
		if(indices.size() > 3) {
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

			//Create a monotone polygon from the indices
			for (size_t i = 0; i < sortedIndices.size(); ++i) {
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
					//Decide the direction in which we are going to 
					//travel across the edges
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

					//Check that the line does not exit the polygon
					if(nBegin < indices.size()) {
						//Get the line which divides the polygon in 2
						const Zuazo::Math::Line<float, 2> divisor(
							polygon[indices[nIth]], 
							polygon[indices[nBegin]]
						);

						//Check if it intersects any edge other than itself
						for(size_t j0 = 0; j0 < indices.size(); ++j0) { 
							const auto j1 = (j0+1) % indices.size();
							if(j0==nIth || j0==nBegin || j1==nIth || j1==nBegin){
								continue;
							}
							
							//Get the current test segment
							const Zuazo::Math::Line<float, 2> polygonSegment(
								polygon[indices[j0]], 
								polygon[indices[j1]]
							);

							//Check intersection
							if(Zuazo::Math::getIntersection(divisor, polygonSegment)) {
								//This segment exits the polygon. Try with another
								nBegin = indices.size();
								break;
							}
						}
					}

					//Extract a monotone polygon if the previous calculations were successful
					if(nBegin < indices.size()) {
						//Ensure ordering to step over all elements in between nBegin and nEnd
						const size_t nEnd = min(nBegin, nIth)+1;
						nBegin = max(nBegin, nIth);
						assert(nEnd < nBegin);

						//Obtain the begin and end iterators for the edge range
						const auto begin = std::next(indices.cbegin(), nBegin);
						const auto end = std::next(indices.cbegin(), nEnd);

						//Add all the vertices in a circular manner. Remove all but the first and last
						std::copy(begin, indices.cend(), std::back_inserter(monotonePolygonIndices));
						std::copy(indices.cbegin(), end, std::back_inserter(monotonePolygonIndices));

						//Discard all the edges but the first and last. Same manner as above
						indices.erase(std::next(begin), indices.cend());
						indices.erase(indices.cbegin(), std::prev(end));

						//We have finished creating a monotone polygon
						break;
					}
				}
			}
		}

		//The last step consists on dumping the rest of the indices
		if(monotonePolygonIndices.empty()) {
			monotonePolygonIndices = std::move(indices);
			assert(indices.empty());
		}
		assert(!monotonePolygonIndices.empty());

		//Ensure that at least a triangle was formed
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