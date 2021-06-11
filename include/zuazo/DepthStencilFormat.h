#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>
#include <ostream>

namespace Zuazo {


enum class DepthStencilFormat {
	none = -1,

	D16,
	X8D24_32,
	D32f,
	S8,
	D16S8,
	D24S8,
	D32fS8,

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(DepthStencilFormat)
ZUAZO_ENUM_COMP_OPERATORS(DepthStencilFormat)	

constexpr std::string_view toString(DepthStencilFormat format) noexcept;
size_t fromString(std::string_view str, DepthStencilFormat& format);
std::ostream& operator<<(std::ostream& os, DepthStencilFormat format);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<DepthStencilFormat> {
	static constexpr DepthStencilFormat first() noexcept;
	static constexpr DepthStencilFormat last() noexcept;
};

}

}

#include "DepthStencilFormat.inl"
