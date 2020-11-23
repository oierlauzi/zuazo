#pragma once 

#include "Math/Vector.h"

#include <string_view>

namespace Zuazo {

enum class ScalingMode {
	NONE = -1,

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
struct EnumTraits;

template<>
struct EnumTraits<ScalingMode> {
	static constexpr ScalingMode first() noexcept;
	static constexpr ScalingMode last() noexcept;
};

}

}

#include "ScalingMode.inl"