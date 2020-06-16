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

}

#include "ScalingFilter.inl"