#pragma once

#include "Macros.h"

#include <string_view>
#include <ostream>

namespace Zuazo {

enum class BlendingMode {
	none = -1,			///<C = C_dst

	write,				///<C = C_src
	opacity,			///<C = C_src * A_src + C_dst * (1 - A_src)
	add,				///<C = C_src * A_src + C_dst
	differenceInv,		///<C = C_dst - C_src * A_src
	difference,			///<C = C_src * A_src - C_dst
	darken,				///<C = min(C_src * A_src, C_dst)
	lighten,			///<C = max(C_src * A_src, C_dst)
	multiply,			///<C = (C_src * A_src) * C_dst
	screen,				///<1 - C = (1 - C_src * A_src) * (1 - C_dst) => C = C_src * A_src + C_dst * (1 - C_src * A_src)

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(BlendingMode)
ZUAZO_ENUM_COMP_OPERATORS(BlendingMode)	

constexpr std::string_view toString(BlendingMode mode) noexcept;
bool fromString(std::string_view str, BlendingMode& mode);
std::ostream& operator<<(std::ostream& os, BlendingMode mode);

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

