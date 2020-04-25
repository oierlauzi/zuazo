#pragma once

#include "Macros.h"
#include "Resolution.h"

#include <type_traits>
#include <string_view>

namespace Zuazo {

/**
 * ColorSubsampling defines how much chroma componentes (Cb ad Cr) are subsampled
 */
enum class ColorSubsampling {
	NONE = 0,					///<Not supported

	CHROMA_444,					///<No subsampling
	CHROMA_422,					///<1 luminance sample per 2 chominance samples on even and odd lines.
	CHROMA_420,					///<1 luminance sample per 2 chominance samples on even lines. No chominance samples on odd lines.
	CHROMA_411,					///<1 luminance sample per 4 chominance samples on even and odd lines.
	CHROMA_410,					///<1 luminance sample per 4 chominance samples on even lines. No chominance samples on odd lines.
	CHROMA_311,					///<1 luminance sample per 3 chominance samples on even and odd lines.
	BAYER,						///<RGB color is stored as a Bayer pattern. 1 R and 1 B per 2 G

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorSubsampling)
ZUAZO_ENUM_COMP_OPERATORS(ColorSubsampling)	

constexpr std::string_view toString(ColorSubsampling colorSubsampling);
constexpr Resolution getSubsampledResolution(ColorSubsampling subs, Resolution res);

}

#include "ColorSubsampling.inl"