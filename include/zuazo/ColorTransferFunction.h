#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorTransferFunction represents how color components are encoded in their numeric values
 */
enum class ColorTransferFunction {
	none = -1,					///<Not supported

	linear,						///<Linearly encoded
	bt1886,						///<Transfer function defined by ITU-R BT.1886. 
								///<Used by BT.601, BT.709, BT.2020. 
								///<\note Although the original standard is specified for 
								///<depths upto 10bits, the extendended version provided by
								///<BT.2020 has been adopted. 
								///<\see https://www.itu.int/rec/R-REC-BT.1886/en
								///<\see https://www.itu.int/rec/R-REC-BT.2020/en
	gamma22,					///<2.2 gamma function
	gamma26,					///<2.6 gamma function
	gamma28,					///<2.8 gamma function
	iec61966_2_1,				///<Non-linearly encoded according to the IEC 61966-2-1 standard. Used in sRGB and Display P3 color spaces.
	iec61966_2_4,				///<Non-linearly encoded according to the IEC 61966-2-4 standard. Similar to BT709, but with support out of [0-1] range
	smpte240M,					///<
	smpte2084,					///<Also known as PQ
	aribStdB67,					///<Also known as HLG or hybrid log-gamma
			
	
	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorTransferFunction)
ZUAZO_ENUM_COMP_OPERATORS(ColorTransferFunction)	
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc) noexcept;
size_t fromString(std::string_view str, ColorTransferFunction& trf);
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
