#pragma once

#include "Macros.h"
#include "Resolution.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorSubsampling defines how less "important" color components are subsampled
 */
enum class ColorSubsampling {
	none = -1,					///<Not supported

	rb444,						///<No subsampling
	rb440,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	rb422,						///<1 G (or Y) sample per 2 R (or Cr) and B (or Cb) samples on even and odd lines.
	rb420,						///<1 G (or Y) sample per 2 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	rb411,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even and odd lines.
	rb410,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	rb311,						///<1 G (or Y) sample per 3 R (or Cr) and B (or Cb) samples on even and odd lines.

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorSubsampling)
ZUAZO_ENUM_COMP_OPERATORS(ColorSubsampling)	

constexpr Math::Vec2i getSubsamplingFactor(ColorSubsampling subs) noexcept;
constexpr Resolution getSubsampledResolution(ColorSubsampling subs, Resolution res) noexcept;

constexpr std::string_view toString(ColorSubsampling colorSubsampling) noexcept;
bool fromString(std::string_view str, ColorSubsampling& subsampling);
std::ostream& operator<<(std::ostream& os, ColorSubsampling colorSubsampling);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorSubsampling> {
	static constexpr ColorSubsampling first() noexcept;
	static constexpr ColorSubsampling last() noexcept;
};

}

}

#include "ColorSubsampling.inl"
