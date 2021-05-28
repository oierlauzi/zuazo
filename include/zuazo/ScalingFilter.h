#pragma once 

#include <string_view>

namespace Zuazo {

enum class ScalingFilter {
	none = -1,

	nearest,
	linear,
	cubic,

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ScalingFilter)
ZUAZO_ENUM_COMP_OPERATORS(ScalingFilter)

constexpr std::string_view toString(ScalingFilter filter) noexcept;
bool fromString(std::string_view str, ScalingFilter& filter);
std::ostream& operator<<(std::ostream& os, ScalingFilter res);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ScalingFilter> {
	static constexpr ScalingFilter first() noexcept;
	static constexpr ScalingFilter last() noexcept;
};
}

}

#include "ScalingFilter.inl"
