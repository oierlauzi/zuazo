#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorTransferFunction represents how color components are encoded in their numeric values
 */
enum class ColorTransferFunction {
	NONE = 0,					///<Not supported

	LINEAR,						///<Linearly encoded
	IEC61966_2_1,				///<Non-linearly encoded according to the IEC 61966-2-1 standard. Used in sRGB and Display P3 color spaces.
	
	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorTransferFunction)
ZUAZO_ENUM_COMP_OPERATORS(ColorTransferFunction)	
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc);
std::ostream& operator<<(std::ostream& os, ColorTransferFunction colorTransferFunc);

namespace Utils {

template<typename T>
class Any;

constexpr ColorTransferFunction lowest(const Any<ColorTransferFunction>& any);
constexpr ColorTransferFunction highest(const Any<ColorTransferFunction>& any);

}

}

#include "ColorTransferFunction.inl"