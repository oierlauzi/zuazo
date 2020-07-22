#include "ColorPrimaries.h"

namespace Zuazo {

constexpr Math::Mat4x4f getConversionMatrix(ColorPrimaries colorPrim){
	switch(colorPrim){
		
	//This matrices are defined in column major order.
	case ColorPrimaries::BT709: 
		return Math::Mat4x4f(
			0.412391,	0.212639f,	0.019331f,	0.0f,
			0.357584f,	0.715169f,	0.119195f,	0.0f,
			0.180481f,	0.072192f,	0.950532f,	0.0f,
			0.0f, 		0.0f,		0.0f,		1.0f
		);
	case ColorPrimaries::BT2020: 
		return Math::Mat4x4f(
			0.636958f,	0.262700f,	0.0f,		0.0f,
			0.144617f,	0.677998f,	0.028073f,	0.0f,
			0.168881f,	0.059302f,	1.060985f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		);
	case ColorPrimaries::SMPTE432: 
		return Math::Mat4x4f(
			0.486571f,	0.228975f,	0.0f,		0.0f,
			0.265668f,	0.691738f,	0.045113f,	0.0f,
			0.198217f,	0.079287f,	1.043945f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		);
	case ColorPrimaries::ADOBE_RGB: 
		return Math::Mat4x4f(
			0.576669f,	0.297345f,	0.027031f,	0.0f,
			0.185558f,	0.627364f,	0.070689f,	0.0f,
			0.188229f,	0.075291f,	0.991338f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		);

	default: return {};
	}
}



constexpr std::string_view toString(ColorPrimaries colorPrim){
	switch(colorPrim){

	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, SMPTE432 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, ADOBE_RGB )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ColorPrimaries colorPrim) {
	return os << toString(colorPrim);
}


namespace Utils {

constexpr ColorPrimaries lowest(const Any<ColorPrimaries>&) {
	return ColorPrimaries::NONE + static_cast<ColorPrimaries>(1);
}

constexpr ColorPrimaries highest(const Any<ColorPrimaries>&) {
	return ColorPrimaries::COUNT - static_cast<ColorPrimaries>(1);
}

}

}