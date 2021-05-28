#pragma once

#include "Macros.h"

#include <ostream>

namespace Zuazo {

/**
 * ColorChromaLocation where the chroma (Cb Cr) samples are located
 * if they are subsampled. This value should be specified per axis
 */
enum class ColorChromaLocation {
	none = -1,					///<Not supported

	cosited0,					///<The chroma sample is on the same spot as the first luma samples (modulo subsampling factor).
	midpoint,					///<The chroma sample halfway between the even and odd (0-based) luma samples.

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorChromaLocation)
ZUAZO_ENUM_COMP_OPERATORS(ColorChromaLocation)



constexpr std::string_view toString(ColorChromaLocation colorChromaLoc) noexcept;
bool fromString(std::string_view str, ColorChromaLocation& location);
std::ostream& operator<<(std::ostream& os, ColorChromaLocation location);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorChromaLocation> {
	static constexpr ColorChromaLocation first() noexcept;
	static constexpr ColorChromaLocation last() noexcept;
};

}

}

#include "ColorChromaLocation.inl"
