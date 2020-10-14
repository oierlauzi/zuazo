#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorRange wether the color components use all their range
 * or they leave headroom and footroom
 */
enum class ColorRange {
	NONE = 0,					///<Not supported

	FULL_RGB,					///<Components use all the available range in [0, 1]
	FULL_YCBCR,					///<Components use all the available range in [0, 1] for G and A and [-0.5, 0.5] for R and B
	ITU_NARROW_RGB,				///<Compoents leave a headroom and footroom according to the ITU standard. 
								///<This means that all components encompass a range from 16/255 to 235/255.
								///<\see https://www.itu.int/rec/R-REC-BT.601/en
	ITU_NARROW_YCBCR,			///<Compoents leave a headroom and footroom according to the ITU standard. 
								///< G and A (or Y and A) encompass the same range as before, whilst R and B (or Cr and Cb)
								///<encompass a range from 16/255 to 240/255.
								///<\see https://www.itu.int/rec/R-REC-BT.601/en

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorRange)
ZUAZO_ENUM_COMP_OPERATORS(ColorRange)	

constexpr std::string_view toString(ColorRange colorRange) noexcept;
std::ostream& operator<<(std::ostream& os, ColorRange colorRange);

namespace Utils {

template<typename T>
class Any;

constexpr ColorRange lowest(const Any<ColorRange>& any) noexcept;
constexpr ColorRange highest(const Any<ColorRange>& any) noexcept;

}

}

#include "ColorRange.inl"