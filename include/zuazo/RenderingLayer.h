#pragma once

#include "Macros.h"

#include <string_view>
#include <ostream>

namespace Zuazo {

enum class RenderingLayer {
	none = -1,
	
	background,
	scene,
	foreground,

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(RenderingLayer)
ZUAZO_ENUM_COMP_OPERATORS(RenderingLayer)	

constexpr std::string_view toString(RenderingLayer layer) noexcept;
size_t fromString(std::string_view str, RenderingLayer& layer);
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
