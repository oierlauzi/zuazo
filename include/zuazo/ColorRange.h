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
	none = -1,					///<Not supported

	full,						///<Components use all the available range in [0, 1], [-0.5, 0.5] if chroma samples
								///<  (Cb and Cr)
	ituNarrow,					///<Components leave a headroom and footroom according to the ITU standard. 
								///<This means that components encompass a range from 16/255 to 235/255 or 16/255 
								///< to 240/255 for chroma samples (Cb and Cr)
								///<\see https://www.itu.int/rec/R-REC-BT.601/en
	ituNarrowFullAlpha,			///<Same as above but alpha channel uses all the available range

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorRange)
ZUAZO_ENUM_COMP_OPERATORS(ColorRange)	

constexpr std::string_view toString(ColorRange colorRange) noexcept;
bool fromString(std::string_view str, ColorRange& range);
std::ostream& operator<<(std::ostream& os, ColorRange colorRange);

namespace Utils {

template<typename T>
class Any;

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorRange> {
	static constexpr ColorRange first() noexcept;
	static constexpr ColorRange last() noexcept;
};
}

}

#include "ColorRange.inl"
