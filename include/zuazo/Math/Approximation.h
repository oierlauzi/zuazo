#pragma once

#include <limits>

namespace Zuazo::Math {

template <typename T>
constexpr bool approxEqual(	const T& lhs, 
							const T& rhs, 
							const T& epsilon = 256*std::numeric_limits<T>::epsilon() ) noexcept;

template <typename T>
constexpr T approxZero(	const T& num, 
						const T& epsilon = 256*std::numeric_limits<T>::epsilon() ) noexcept;

}

#include "Approximation.inl"