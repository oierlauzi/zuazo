#include "Approximation.h"

#include "Absolute.h"

namespace Zuazo::Math {

template <typename T>
constexpr bool approxEqual(	const T& lhs, 
							const T& rhs,
							const T& epsilon ) noexcept 
{
	return abs(rhs - lhs) < epsilon;
}

template <typename T>
constexpr T approxZero(	const T& num, 
						const T& epsilon ) noexcept 
{
	return abs(num) > epsilon ? num : T(0);
}

}