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

	FULL,						///<Components use all the available range
	ITU_NARROW_RGB,				///<Compoents leave a headroom and footroom according to the ITU standard. 
								///<This means that ALL components encompass a range from 16/255 to 235/255
								///<\see https://www.itu.int/rec/R-REC-BT.601/en
	ITU_NARROW_YCBCR,			///<Compoents leave a headroom and footroom according to the ITU standard. 
								///<This means that G and A (or Y and A) components encompass a range from
								///<16/255 to 235/255, same as ITU_NARROW_RGB, whilst R and B (or Cb and Cr)
								///<encompass a range from 16/255 to 240/255
								///<\see https://www.itu.int/rec/R-REC-BT.601/en

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorRange)
ZUAZO_ENUM_COMP_OPERATORS(ColorRange)	

constexpr std::string_view toString(ColorRange colorRange);
std::ostream& operator<<(std::ostream& os, ColorRange colorRange);

namespace Utils {

template<typename T>
class Any;

constexpr ColorRange lowest(const Any<ColorRange>& any);
constexpr ColorRange highest(const Any<ColorRange>& any);

}

}

#include "ColorRange.inl"