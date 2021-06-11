#include "ColorModel.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr bool isYCbCr(ColorModel colorModel) noexcept {
	switch(colorModel){
	case ColorModel::yiq:
	case ColorModel::yuv:
	case ColorModel::bt601:
	case ColorModel::bt709: 
	case ColorModel::bt2020:
	case ColorModel::smpte240M:
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
	case ColorModel::rgb: return Math::Mat3x3f(1.0f); //Identity
	case ColorModel::yiq: 
		return constructRGB2YCbCrConversionMatrix(
			0.299f, 	0.587f, 	0.114f,
			+0.595716f,	-0.274453f,	-0.321263f,
			+0.211456f,	-0.522591f,	+0.311135f
		);
	case ColorModel::yuv: 
		return constructRGB2YCbCrConversionMatrix(
			0.299f, 	0.587f, 	0.114f,
			-0.14713f,	-0.28886f,	+0.436f,
			+0.615f,	-0.51499f,	-0.10001f
		);
	case ColorModel::bt601: return constructRGB2YCbCrConversionMatrix(0.299f, 0.114f);
	case ColorModel::bt709: return constructRGB2YCbCrConversionMatrix(0.2126f, 0.0722f);
	case ColorModel::bt2020: return constructRGB2YCbCrConversionMatrix(0.2627f, 0.0593f);
	case ColorModel::smpte240M: return constructRGB2YCbCrConversionMatrix(0.212f, 0.087f);
	default: return Math::Mat3x3f(0.0f);
	}
}



constexpr std::string_view toString(ColorModel colorModel) noexcept {
	switch(colorModel){

	ZUAZO_ENUM2STR_CASE( ColorModel, rgb )
	ZUAZO_ENUM2STR_CASE( ColorModel, yiq )
	ZUAZO_ENUM2STR_CASE( ColorModel, yuv )
	ZUAZO_ENUM2STR_CASE( ColorModel, bt601 )
	ZUAZO_ENUM2STR_CASE( ColorModel, bt709 )
	ZUAZO_ENUM2STR_CASE( ColorModel, bt2020 )
	ZUAZO_ENUM2STR_CASE( ColorModel, smpte240M )

	default: return "";
	}
}

inline size_t fromString(std::string_view str, ColorModel& model) {
	return enumFromString(str, model);
}

inline std::ostream& operator<<(std::ostream& os, ColorModel model) {
	return os << toString(model);
}


namespace Utils {

constexpr ColorModel EnumTraits<ColorModel>::first() noexcept {
	return ColorModel::none + static_cast<ColorModel>(1);
}

constexpr ColorModel EnumTraits<ColorModel>::last() noexcept {
	return ColorModel::count - static_cast<ColorModel>(1);
}

}

}