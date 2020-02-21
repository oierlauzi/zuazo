#include "ColorModel.h"

namespace Zuazo {

constexpr std::string_view toString(ColorModel colorModel){
	switch(colorModel){

	ZUAZO_ENUM2STR_CASE( ColorModel, RGB )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT601 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorModel, BT2020 )

	default: return "";
	}
}

constexpr Math::Mat4x4f getConversionMatrix(ColorModel colorModel){
	switch(colorModel){

	case ColorModel::RGB: 
		return Math::Mat4x4f(1.0f);
	case ColorModel::BT601: 
		return {
			0.299f,					0.587f,					0.114f,					0.0f,
			-0.14713f,				-0.28886f,				0.436f,					0.0f,
			0.615f,					-0.51499f,				-0.10001f,				0.0f,
			0.0f,					0.0f,					0.0f,					1.0f
		};
	case ColorModel::BT709: 
		return {
			0.212639005871510f,		0.715168678767756f,		0.072192315360734f,		0.0f,
			-0.114592177555732f,	-0.385407822444268f,	0.5f,					0.0f,
			0.5f,					-0.454155517037873f,	-0.045844482962127f,	0.0f,
			0.0f,					0.0f,					0.0f,					1.0f
		};
	case ColorModel::BT2020: 
		return {
			0.262700212011267f,		0.677998071518871f,		0.059301716469862f,		0.0f,
			-0.139630430187157f,	-0.360369569812843f,	0.5f,					0.0f,
			0.5f,					-0.459784529009814f,	-0.040215470990186f,	0.0f,
			0.0f,					0.0f,					0.0f,					1.0f
		};

	default: return {};
	}
}


}