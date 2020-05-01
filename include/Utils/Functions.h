#pragma once

namespace Zuazo::Utils {
	
template<typename T>
constexpr T bit(T pos);

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment);

}

#include "Functions.inl"