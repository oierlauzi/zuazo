#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * ColorModel represents how color gets encoded on a tuple of values or components
 */
enum class ColorModel {
	NONE = 0,					///<Not supported

	MONOCHROME,					///<No color.
	RGB,						///<RGB color model
	BT601,						///<YCbCr color model used in SDTV \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<YCbCr color model used in HDTV \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<YCbCr color model used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorModel)
ZUAZO_ENUM_COMP_OPERATORS(ColorModel)

}