#include "ColorModel.h"

namespace Zuazo {

constexpr bool isYCbCr(ColorModel colorModel) {
	switch(colorModel){
	case ColorModel::BT601:
	case ColorModel::BT709: 
	case ColorModel::BT2020:
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

constexpr Math::Mat4x4f getRGB2YCbCrConversionMatrix(ColorModel colorModel){
	switch(colorModel){

	//This matrices are defined in row major order and then transposed to be in column major
	case ColorModel::RGB: return Math::Mat4x4f(1.0f); //Identity
	case ColorModel::BT601: return constructRGB2YCbCrConversionMatrix(
			0.299f,					0.587f,					0.114f, 	//Y
			-0.14713f,				-0.28886f,				0.436f, 	//Cb
			0.615f,					-0.51499f,				-0.10001f	//Cr
		);
	case ColorModel::BT709: return constructRGB2YCbCrConversionMatrix(
			0.212639005871510f,		0.715168678767756f,		0.072192315360734f,	//Y	
			-0.114592177555732f,	-0.385407822444268f,	0.5f,				//Cb	
			0.5f,					-0.454155517037873f,	-0.045844482962127f	//Cr	
		);
	case ColorModel::BT2020: return constructRGB2YCbCrConversionMatrix(
			0.262700212011267f,		0.677998071518871f,		0.059301716469862f,	//Y 
			-0.139630430187157f,	-0.360369569812843f,	0.5f,				//Cb
			0.5f,					-0.459784529009814f,	-0.040215470990186f	//Cr
		);
	default: return {};
	}
}



constexpr std::string_view toString(ColorModel colorModel){
	switch(colorModel){

	ZUAZO_ENUM2STR_CASE( ColorModel, RGB )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT601 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT2020 )

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