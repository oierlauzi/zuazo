#pragma once 

#include "Math/Vector.h"

#include <string_view>

namespace Zuazo {

enum class ScalingMode {
	NONE,

	STRETCH,
	BOXED,
	CROPPED,
	CLAMP_HORIZONTALLY,
	CLAMP_VERTICALLY,

	COUNT
};


template<typename T>
constexpr Math::Vec2<T> scale(const Math::Vec2<T>& srcSize, const Math::Vec2<T>& dstSize, ScalingMode mode);

constexpr std::string_view toString(ScalingMode mode);

}

#include "ScalingMode.inl"