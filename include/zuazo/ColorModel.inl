#include "ColorModel.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr bool isYCbCr(ColorModel colorModel) noexcept {
	switch(colorModel){
	case ColorModel::YIQ:
	case ColorModel::YUV:
	case ColorModel::BT601:
	case ColorModel::BT709: 
	case ColorModel::BT2020:
	case ColorModel::SMPTE240M:
		return true;
	default: 
		return false;
	}
}

template<typename T>
constexpr Math::Mat3x3<T> constructRGB2YCbCrConversionMatrix(	const T y_r,  const T y_g,  const T y_b,
																const T cb_r, const T cb_g, const T cb_b,
																const T cr_r, const T cr_g, const T cr_b ) noexcept
{
	//		R		G		B		A
	return Math::Mat3x3<T>(
		cr_r,	cr_g,	cr_b,	//Cr (R)
		y_r,	y_g,	y_b, 	//Y  (G)
		cb_r,	cb_g,	cb_b 	//Cb (B)
	);
}

template<typename T>
constexpr Math::Mat3x3<T> constructRGB2YCbCrConversionMatrix(const T k_r,  const T k_b) noexcept {
	const T k_g = T(1) - k_r - k_b; //As k_r + k_g + k_b = 1

	return constructRGB2YCbCrConversionMatrix(
		+k_r, 							+k_g,							+k_b,
		-T(0.5) * k_r / (T(1) - k_b),	-T(0.5) * k_g / (T(1) - k_b),	+T(0.5),
		+T(0.5),						-T(0.5) * k_g / (T(1) - k_r),	-T(0.5) * k_b / (T(1) - k_r)
	);
}

constexpr Math::Mat3x3f getRGB2YCbCrConversionMatrix(ColorModel colorModel) noexcept {
	switch(colorModel){
	
	//From https://en.wikipedia.org/wiki/YCbCr
	case ColorModel::RGB: return Math::Mat3x3f(1.0f); //Identity
	case ColorModel::YIQ: 
		return constructRGB2YCbCrConversionMatrix(
			0.299f, 	0.587f, 	0.114f,
			+0.595716f,	-0.274453f,	-0.321263f,
			+0.211456f,	-0.522591f,	+0.311135f
		);
	case ColorModel::YUV: 
		return constructRGB2YCbCrConversionMatrix(
			0.299f, 	0.587f, 	0.114f,
			-0.14713f,	-0.28886f,	+0.436f,
			+0.615f,	-0.51499f,	-0.10001f
		);
	case ColorModel::BT601: return constructRGB2YCbCrConversionMatrix(0.299f, 0.114f);
	case ColorModel::BT709: return constructRGB2YCbCrConversionMatrix(0.2126f, 0.0722f);
	case ColorModel::BT2020: return constructRGB2YCbCrConversionMatrix(0.2627f, 0.0593f);
	case ColorModel::SMPTE240M: return constructRGB2YCbCrConversionMatrix(0.212f, 0.087f);
	default: return Math::Mat3x3f(0.0f);
	}
}



constexpr std::string_view toString(ColorModel colorModel) noexcept {
	switch(colorModel){

	ZUAZO_ENUM2STR_CASE( ColorModel, RGB )
	ZUAZO_ENUM2STR_CASE( ColorModel, YIQ )
	ZUAZO_ENUM2STR_CASE( ColorModel, YUV )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT601 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorModel, SMPTE240M )

	default: return "";
	}
}

inline bool fromString(std::string_view str, ColorModel& model) {
	return enumFromString(str, model);
}

inline std::ostream& operator<<(std::ostream& os, ColorModel model) {
	return os << toString(model);
}


namespace Utils {

constexpr ColorModel EnumTraits<ColorModel>::first() noexcept {
	return ColorModel::NONE + static_cast<ColorModel>(1);
}

constexpr ColorModel EnumTraits<ColorModel>::last() noexcept {
	return ColorModel::COUNT - static_cast<ColorModel>(1);
}

}

}