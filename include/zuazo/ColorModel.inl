#include "ColorModel.h"

namespace Zuazo {

constexpr bool isYCbCr(ColorModel colorModel) {
	switch(colorModel){
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
constexpr Math::Mat4x4<T> constructRGB2YCbCrConversionMatrix(	const T y_r,  const T y_g,  const T y_b,
																const T cb_r, const T cb_g, const T cb_b,
																const T cr_r, const T cr_g, const T cr_b )
{
	//Transpose the matrix as it is specified in row major order whilst glm uses colum major ordering
	//		R		G		B		A
	return glm::transpose(Math::Mat4x4<T>(
			cr_r,	cr_g,	cr_b,	T(0),	//Cr (R)
			y_r,	y_g,	y_b,	T(0), 	//Y  (G)
			cb_r,	cb_g,	cb_b,	T(0), 	//Cb (B)
			T(0),	T(0),	T(0),	T(1)	//A  (A)
	));
}

template<typename T>
constexpr Math::Mat4x4<T> constructRGB2YCbCrConversionMatrix(const T k_b,  const T k_r) {
	const T k_g = T(1) - k_b - k_r; //As k_r + k_g + k_b = 1
	const T half = T(1) / T(2);

	return constructRGB2YCbCrConversionMatrix(
		+k_r, 						+k_g,						+k_b,
		-half * k_r / (1 - k_b),	-half * k_g / (1 - k_b),	+half,
		+half,						-half * k_g / (1 - k_r),	-half * k_b / (1 - k_r)
	);
}

constexpr Math::Mat4x4f getRGB2YCbCrConversionMatrix(ColorModel colorModel){
	switch(colorModel){

	//This matrices are defined in row major order and then transposed to be in column major
	case ColorModel::RGB: return Math::Mat4x4f(1.0f); //Identity
	case ColorModel::BT601: return constructRGB2YCbCrConversionMatrix(0.299f, 0.114f);
	case ColorModel::BT709: return constructRGB2YCbCrConversionMatrix(0.2126f, 0.0722f);
	case ColorModel::BT2020: return constructRGB2YCbCrConversionMatrix(0.2627f, 0.0593f);
	case ColorModel::SMPTE240M: return constructRGB2YCbCrConversionMatrix(0.212f, 0.087f);
	default: return {};
	}
}



constexpr std::string_view toString(ColorModel colorModel){
	switch(colorModel){

	ZUAZO_ENUM2STR_CASE( ColorModel, RGB )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT601 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorModel, SMPTE240M )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ColorModel model) {
	return os << toString(model);
}


namespace Utils {

constexpr ColorModel lowest(const Any<ColorModel>&) {
	return ColorModel::NONE + static_cast<ColorModel>(1);
}

constexpr ColorModel highest(const Any<ColorModel>&) {
	return ColorModel::COUNT - static_cast<ColorModel>(1);
}

}

}