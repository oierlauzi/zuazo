#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * ColorPrimaries represents which red, green and blue color primaries are used to 
 * sythetise color. This influences the range of representable range of colors
 */
enum class ColorPrimaries {
	NONE = 0,					///<Not supported

	BT709,						///<Used in HDTV and sRGB color space\see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<Used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE432,					///<Used in Display P3 color space
	ADOBE_RGB,					///<Used in Adobe RGB color space

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorPrimaries)
ZUAZO_ENUM_COMP_OPERATORS(ColorPrimaries)	

}