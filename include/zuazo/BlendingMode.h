#pragma once

#include "Macros.h"

#include <string_view>
#include <ostream>

namespace Zuazo {

enum class BlendingMode {
	NONE = 0,			///<C = C_dst

	WRITE,				///<C = C_src
	OPACITY,			///<C = C_src * A_src + C_dst * (1 - A_src)
	ADD,				///<C = C_src + C_dst
	DIFFERENCE,			///<C = C_src - C_dst
	DIFFERENCE_1,		///<C = C_dst - C_src
	DARKEN,				///<C = min(C_src, C_dst)
	LIGHTEN,			///<C = max(C_src, C_dst)
	MULTIPLY,			///<C = C_src * C_dst
	SCREEN,				///<1 - C = (1 - C_src) * (1 - C_dst) => C = C_src + C_dst * (1 - C_src)

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(BlendingMode)
ZUAZO_ENUM_COMP_OPERATORS(BlendingMode)	

constexpr std::string_view toString(BlendingMode format) noexcept;
std::ostream& operator<<(std::ostream& os, BlendingMode format);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<BlendingMode> {
	static constexpr BlendingMode first() noexcept;
	static constexpr BlendingMode last() noexcept;
};

}

}

#include "BlendingMode.inl"
