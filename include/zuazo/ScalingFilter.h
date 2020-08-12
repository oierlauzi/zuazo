#pragma once 

#include <string_view>

namespace Zuazo {

enum class ScalingFilter {
	NONE,

	NEAREST,
	LINEAR,

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ScalingFilter)
ZUAZO_ENUM_COMP_OPERATORS(ScalingFilter)

constexpr std::string_view toString(ScalingFilter filter);
std::ostream& operator<<(std::ostream& os, ScalingFilter res);

namespace Utils {

template<typename T>
class Any;

constexpr ScalingFilter lowest(const Any<ScalingFilter>& any);
constexpr ScalingFilter highest(const Any<ScalingFilter>& any);

}

}

#include "ScalingFilter.inl"