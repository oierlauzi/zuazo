#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * ColorEncoding represents how color components are encoded in it's allocated space
 */
enum class ColorEncoding {
	NONE = 0,					///<Not supported

	UINT,						///<Linearly encoded on a unsigned integer
	UINT_NARROW,				///<Linearly encided on a unsigned integer, leaving head-room and foot-room. Available range: 219*2^(n-8)
  	INT,						///<Linearly encoded on a integer (2's complement)
	IEEE754,					///<Linearly encoded on a floating point number
	IEC61966_2_1,				///<Non-linearly encoded on a unsigned integer according to the IEC 61966-2-1 standard. Used in sRGB and Display P3 color spaces.

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorEncoding)
ZUAZO_ENUM_COMP_OPERATORS(ColorEncoding)	
	
}