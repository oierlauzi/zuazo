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

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ScalingMode)
ZUAZO_ENUM_COMP_OPERATORS(ScalingMode)

template<typename T>
constexpr Math::Vec2<T> scale(const Math::Vec2<T>& srcSize, const Math::Vec2<T>& dstSize, ScalingMode mode) noexcept;

constexpr std::string_view toString(ScalingMode mode) noexcept;
std::ostream& operator<<(std::ostream& os, ScalingMode scalingMode);

namespace Utils {

template<typename T>
class Any;

constexpr ScalingMode lowest(const Any<ScalingMode>& any) noexcept;
constexpr ScalingMode highest(const Any<ScalingMode>& any) noexcept;

}

}

#include "ScalingMode.inl"