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
	ITU_NARROW,					///<Compoents leave a headroom and footroom according to the ITU standard. 
								///<This means that if the ColorModel is RGB all components encompass a range 
								///<from 16/255 to 235/255. If it is a YCbCr ColorModel is chosen, G and A 
								///<(or Y and A) encompass the same range as before, whilst R and B (or Cr and Cb)
								///<encompass a range from 16/255 to 240/255.
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