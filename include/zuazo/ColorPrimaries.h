#pragma once

#include "Macros.h"
#include "Math/Matrix.h"

#include <type_traits>
#include <string_view>
#include <ostream>

namespace Zuazo {

/**
 * ColorPrimaries represents which red, green and blue color primaries are used to 
 * sythetise color. This influences the range of representable range of colors
 */
enum class ColorPrimaries {
	NONE = 0,					///<Not supported

	BT601_625,					///<Used in PAL and SECAM \see https://www.itu.int/rec/R-REC-BT.601/en
	BT601_525,					///<Used in NTSC \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<Used in HDTV and sRGB color space\see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<Used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE431,					///<Used in DCI-P3 color space
	SMPTE432,					///<Used in Display P3 color space
	ADOBE_RGB,					///<Used in Adobe RGB color space

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorPrimaries)
ZUAZO_ENUM_COMP_OPERATORS(ColorPrimaries)	

template<typename T>
constexpr Math::Mat4x4<T> constructRGB2XYZConversionMatrix(	const T white_x,const T white_y,
															const T red_x,	const T red_y,
															const T green_x,const T green_y,
															const T blue_x, const T blue_y,
															const T white_luminance = T(1) ) noexcept;

constexpr Math::Mat4x4f getRGB2XYZConversionMatrix(ColorPrimaries colorPrim) noexcept;

constexpr std::string_view toString(ColorPrimaries colorPrim) noexcept;
std::ostream& operator<<(std::ostream& os, ColorPrimaries colorPrim);

namespace Utils {

template<typename T>
class Any;

constexpr ColorPrimaries lowest(const Any<ColorPrimaries>& any) noexcept;
constexpr ColorPrimaries highest(const Any<ColorPrimaries>& any) noexcept;

}

}

#include "ColorPrimaries.inl"