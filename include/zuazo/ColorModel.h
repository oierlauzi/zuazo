#pragma once

#include "Macros.h"
#include "Math/Matrix.h"

#include <type_traits>
#include <string_view>
#include <ostream>

namespace Zuazo {

/**
 * ColorModel represents how color gets encoded on a tuple of values or components
 */
enum class ColorModel {
	NONE = 0,					///<Not supported

	RGB,						///<RGB color model
	BT601,						///<YCbCr color model used in SDTV \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<YCbCr color model used in HDTV \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<YCbCr color model used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE240M,

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorModel)
ZUAZO_ENUM_COMP_OPERATORS(ColorModel)

constexpr bool isYCbCr(ColorModel colorModel) noexcept;

template<typename T>
constexpr Math::Mat4x4<T> constructRGB2YCbCrConversionMatrix(	const T y_r,  const T y_g,  const T y_b,
																const T cb_r, const T cb_g, const T cb_b,
																const T cr_r, const T cr_g, const T cr_b ) noexcept;

template<typename T>
constexpr Math::Mat4x4<T> constructRGB2YCbCrConversionMatrix(const T k_r,  const T k_b) noexcept;

constexpr Math::Mat4x4f getRGB2YCbCrConversionMatrix(ColorModel colorModel) noexcept;

constexpr std::string_view toString(ColorModel colorModel) noexcept;
std::ostream& operator<<(std::ostream& os, ColorModel model);

namespace Utils {

template<typename T>
class Any;

constexpr ColorModel lowest(const Any<ColorModel>& any) noexcept;
constexpr ColorModel highest(const Any<ColorModel>& any) noexcept;

}

}

#include "ColorModel.inl"