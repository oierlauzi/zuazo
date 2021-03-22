#pragma once

#include "Macros.h"

#include <string_view>
#include <ostream>

namespace Zuazo {

enum class RenderingLayer {
	NONE = -1,
	
	BACKGROUND,
	SCENE,
	FOREGROUND,

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(RenderingLayer)
ZUAZO_ENUM_COMP_OPERATORS(RenderingLayer)	

constexpr std::string_view toString(RenderingLayer layer) noexcept;
std::ostream& operator<<(std::ostream& os, RenderingLayer layer);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<RenderingLayer> {
	static constexpr RenderingLayer first() noexcept;
	static constexpr RenderingLayer last() noexcept;
};

}

}

#include "RenderingLayer.inl"