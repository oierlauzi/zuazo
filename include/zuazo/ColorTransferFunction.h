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
	BT601,						///<Transfer function defined by ITU-R BT.601. Used on PAL, SECAM and NTSC SDTV systems. \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<Transfer function defined by ITU-R BT.709. Used on HDTV systems. Functionaly equivalent to the above one. \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020_10,					///<Transfer function defined by ITU-R BT.2020 for 10bits per component. Functionaly equivalent to the above one. Used on UHDTV systems. \see https://www.itu.int/rec/R-REC-BT.2020/en
	BT2020_12,					///<Transfer function defined by ITU-R BT.2020 for 12bits per component. Used on UHDTV systems. Functionaly similar to the above one, higher precision constants used. \see https://www.itu.int/rec/R-REC-BT.2020/en
	GAMMA22,					///<2.2 gamma function
	GAMMA26,					///<2.6 gamma function
	GAMMA28,					///<2.8 gamma function
	IEC61966_2_1,				///<Non-linearly encoded according to the IEC 61966-2-1 standard. Used in sRGB and Display P3 color spaces.
	IEC61966_2_4,				///<Non-linearly encoded according to the IEC 61966-2-4 standard. Similar to BT709, but with support out of [0-1] range
	SMPTE240M,					///<
	SMPTE2084,					///<Also known as PQ
	ARIB_STD_B67,				///<Also known as HLG or hybrid log-gamma
			
	
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