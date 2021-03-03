#include "Geometry.h"

#include "Absolute.h"
#include "Approximation.h"

namespace Zuazo::Math {

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(const Vec2<T> vec) noexcept {
	return getAlignmentMatrix(normalize(vec), normalized);
}

template<typename T>
constexpr Mat2x2<typename Vec2<T>::value_type> getAlignmentMatrix(	const Vec2<T> vec,
																	normalized_t ) noexcept
{
	const auto c = vec.x; //cos: x
	const auto s = vec.y; //sin: y

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

template<typename T, size_t N>
constexpr typename Vec<T, N>::value_type distance2(const Vec<T, N>& a, const Vec<T, N>& b) noexcept {
	return length2(b - a);
}

template<typename T, size_t N>
constexpr typename Line<T, N>::value_type proj(const Line<T, N>& line, const typename Line<T, N>::value_type& p) noexcept {
	return line.front() + proj(line.back() - line.front(), p - line.front());
}

template<typename T>
constexpr typename Line<T, 2>::value_type::value_type getSignedDistance(const Line<T, 2>& line,
																		const typename Line<T, 2>::value_type& point ) noexcept
{
	const auto delta = point - line.front();
	const auto direction = normalize(line.back() - line.front());
	return zCross(direction, delta);
}

template<typename T>
constexpr typename Polygon<T>::value_type::value_type getSignedArea(const Polygon<T>& polygon) noexcept {
	auto result = typename Polygon<T>::value_type::value_type();

	//From:
	//https://mathworld.wolfram.com/PolygonArea.html

	for(size_t i = 0; i < polygon.getSegmentCount(); ++i) {
		const auto& segment = polygon.getSegment(i);

		result += zCross(segment.front(), segment.back());
	}

	return result / 2;
}

template<typename T>
constexpr typename Vec2<T>::value_type getSignedArea(	const Vec2<T>& v0,
														const Vec2<T>& v1,
														const Vec2<T>& v2 ) noexcept
{
	const auto c1 = zCross(v0, v1);
	const auto c2 = zCross(v1, v2);
	const auto c3 = zCross(v2, v0);

	return (c1 + c2 + c3) / 2;
}


template<typename T>
constexpr typename Vec2<T>::value_type getWinding(	const Vec2<T>& v0,
													const Vec2<T>& v1,
													const Vec2<T>& v2 ) noexcept
{
	typename Vec2<T>::value_type result = 0;

	const auto c0 = zCross(v0, v2);
	const auto c1 = zCross(v0, v1);
	const auto c2 = zCross(v1, v2);

	if(c0) {
		const auto cond0 = c0 > 0 && (c1 >= 0 && c2 >= 0);
		const auto cond1 = c0 < 0 && (c1 >= 0 || c2 >= 0);

		result = (cond0 || cond1) ? +1 : -1;
	}
}



template<typename T>
constexpr bool isConvex(const Polygon<T>& polygon) noexcept {
	//Triangles are always convex. Only run the algorithm if
	//the input is anything more complex than it
	if(polygon.size() > 3) {
		auto direction = typename Polygon<T>::value_type::value_type();

		for(size_t i = 0; i < polygon.getSegmentCount(); ++i) {
			//Obtain 2 adjacent edges
			const auto& seg0 = polygon.getSegment(i);
			const auto& seg1 = polygon.getSegment((i+1) % polygon.getSegmentCount());

			//Calculate the the z component of the cross 
			//product of adjacent edges
			const auto delta0 = seg0.getDelta();
			const auto delta1 = seg1.getDelta();
			const auto z = zCross(delta0, delta1);

			//Test only if the edges were not colinear
			if(!approxZero(z)) {
				const auto newDirection = sign(z);
				assert(newDirection); //It must not be zero or NaN at this point

				if(!direction) {
					//Direction was not set. Set it
					//with the current edge
					direction = newDirection;
				} else {
					//If direction has changed, this is 
					//not a convex polygon
					if(direction != newDirection) {
						return false;
					}
				}
			}
		}
	}

	//All test passed. This is a convex polygon
	return true;
}

template<typename T>
constexpr bool isConvex(const Vec2<T>& v0,
						const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& v3 ) noexcept
{
	//A quad will be convex if their diagonals intersect
	return getIntersection(
		Line<T, 2>(v0, v2),
		Line<T, 2>(v1, v3)
	);
}



template<typename T>
constexpr bool isInCone(const Vec2<T>& v0,
						const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& p ) noexcept
{
	return isInCone(v1-v0, v2-v0, p-v0);
}

template<typename T>
constexpr bool isInCone(const Vec2<T>& v1,
						const Vec2<T>& v2,
						const Vec2<T>& p ) noexcept
{
	bool result;

	const auto c0 = zCross(v1, v2);
	const auto c1 = zCross(v1, p);
	const auto c2 = zCross(v2-v1, p-v1);

	if(c0 >= 0 && c1 >= 0 && c2 >= 0) {
		result = true;
	} else if (c0 < 0 && (c1 >= 0 || c2 >= 0)) {
		result = true;
	} else {
		result = false;
	}

	return result;
}



template<typename T>
constexpr bool isInsideTriangle(const Vec2<T>& t0,
								const Vec2<T>& t1,
								const Vec2<T>& t2,
								const Vec2<T>& v ) noexcept
{
	return isInsideTriangle(t1-t0, t2-t0, v-t0);
}

template<typename T>
constexpr bool isInsideTriangle(const Vec2<T>& t1,
								const Vec2<T>& t2,
								const Vec2<T>& v ) noexcept
{
	//Obtain the signed areas formed by each side 
	//and the point
	const auto sArea0 = zCross(t1/*-t0*/, v/*-t0*/);
	const auto sArea1 = zCross(t2-t1	, v-t1);
	const auto sArea2 = zCross(/*t0*/-t2, v-t2);

	//Obtain the signs
	const bool hasPos = sArea0 > 0 || sArea1 > 0 || sArea2 > 0;
	const bool hasNeg = sArea0 < 0 || sArea1 < 0 || sArea2 < 0;

	//All of the signed areas should have the same sign.
	return !(hasPos && hasNeg);
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

		//The same value should be obtained from the other line
		assert(!approxZero(length2(*point-b(t))));
	}

	return result;
}

}