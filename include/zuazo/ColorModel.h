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
	none = -1,					///<Not supported

	rgb,						///<RGB color model
	yiq,						///<YIQ color encoding, used in NTSC
	yuv,						///<YUV color encoding, used in PAL
	bt601,						///<YCbCr color model used in SDTV \see https://www.itu.int/rec/R-REC-BT.601/en
	bt709,						///<YCbCr color model used in HDTV \see https://www.itu.int/rec/R-REC-BT.709/en
	bt2020,						///<YCbCr color model used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	smpte240M,

	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorModel)
ZUAZO_ENUM_COMP_OPERATORS(ColorModel)

constexpr bool isYCbCr(ColorModel colorModel) noexcept;

template<typename T>
constexpr Math::Mat3x3<T> constructRGB2YCbCrConversionMatrix(	const T y_r,  const T y_g,  const T y_b,
																const T cb_r, const T cb_g, const T cb_b,
																const T cr_r, const T cr_g, const T cr_b ) noexcept;

template<typename T>
constexpr Math::Mat3x3<T> constructRGB2YCbCrConversionMatrix(const T k_r,  const T k_b) noexcept;

constexpr Math::Mat3x3f getRGB2YCbCrConversionMatrix(ColorModel colorModel) noexcept;

constexpr std::string_view toString(ColorModel colorModel) noexcept;
bool fromString(std::string_view str, ColorModel& model);
std::ostream& operator<<(std::ostream& os, ColorModel model);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorModel> {
	static constexpr ColorModel first() noexcept;
	static constexpr ColorModel last() noexcept;
};

}

}

#include "ColorModel.inl"
