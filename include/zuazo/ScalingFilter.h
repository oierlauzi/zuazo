#pragma once 

#include <string_view>

namespace Zuazo {

enum class ScalingFilter {
	NONE,

	NEAREST,
	LINEAR,

	COUNT
};

constexpr std::string_view toString(ScalingFilter filter);
std::ostream& operator<<(std::ostream& os, ScalingFilter res);

}

#include "ScalingFilter.inl"