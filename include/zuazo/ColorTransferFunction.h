#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorTransferFunction represents how color components are encoded in their numeric values
 */
enum class ColorTransferFunction {
	NONE = -1,					///<Not supported

	LINEAR,						///<Linearly encoded
	BT1886,						///<Transfer function defined by ITU-R BT.1886. 
								///<Used by BT.601, BT.709, BT.2020. 
								///<\note Although the original standard is specified for 
								///<depths upto 10bits, the extendended version provided by
								///<BT.2020 has been adopted. 
								///<\see https://www.itu.int/rec/R-REC-BT.1886/en
								///<\see https://www.itu.int/rec/R-REC-BT.2020/en
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
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc) noexcept;
std::ostream& operator<<(std::ostream& os, ColorTransferFunction colorTransferFunc);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorTransferFunction> {
	static constexpr ColorTransferFunction first() noexcept;
	static constexpr ColorTransferFunction last() noexcept;
};

}

}

#include "ColorTransferFunction.inl"