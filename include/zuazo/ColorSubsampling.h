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
	NONE = 0,					///<Not supported

	RB_444,						///<No subsampling
	RB_440,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	RB_422,						///<1 G (or Y) sample per 2 R (or Cr) and B (or Cb) samples on even and odd lines.
	RB_420,						///<1 G (or Y) sample per 2 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	RB_411,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even and odd lines.
	RB_410,						///<1 G (or Y) sample per 4 R (or Cr) and B (or Cb) samples on even lines. No R and B samples on odd lines.
	RB_311,						///<1 G (or Y) sample per 3 R (or Cr) and B (or Cb) samples on even and odd lines.

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorSubsampling)
ZUAZO_ENUM_COMP_OPERATORS(ColorSubsampling)	

constexpr Resolution getSubsampledResolution(ColorSubsampling subs, Resolution res);

constexpr std::string_view toString(ColorSubsampling colorSubsampling);
std::ostream& operator<<(std::ostream& os, ColorSubsampling colorSubsampling);

namespace Utils {

template<typename T>
class Any;

constexpr ColorSubsampling lowest(const Any<ColorSubsampling>& any);
constexpr ColorSubsampling highest(const Any<ColorSubsampling>& any);

}

}

#include "ColorSubsampling.inl"