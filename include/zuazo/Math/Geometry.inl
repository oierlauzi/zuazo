#include "Geometry.h"

#include "Absolute.h"
#include "Approximation.h"

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

		//The same value should be obtained from the other line
		assert(!approxZero(length2(*point-b(t))));
	}

	return result;
}

}