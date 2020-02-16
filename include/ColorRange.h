#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * ColorRange wether the color components use all their range
 * or they leave headroom and footroom
 */
enum class ColorRange {
	NONE = 0,					///<Not supported

	FULL,						///<Components use all the available range
	ITU_NARROW,					///<Compoents leave a headroom and footroom according to the ITU standard. 
								///<This means that Y, R, G, B encompass a range from 16 to 235 (219*2^n-8 levels) and
								///<Cb, Cr encompass a range from 16 to 240 (224*2^n-8 levels)
								///<\see https://www.itu.int/rec/R-REC-BT.601/en

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorRange)
ZUAZO_ENUM_COMP_OPERATORS(ColorRange)	
	
}