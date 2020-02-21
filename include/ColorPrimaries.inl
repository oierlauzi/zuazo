#include "ColorPrimaries.h"

namespace Zuazo {

constexpr std::string_view toString(ColorPrimaries colorPrim){
	switch(colorPrim){

	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, SMPTE432 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, ADOBE_RGB )

	default: return "";
	}
}

constexpr Math::Mat4x4f getConversionMatrix(ColorPrimaries colorPrim){
	switch(colorPrim){
	
	case ColorPrimaries::BT709: 
		return {
			41.2391,	21.2639f,	1.9331f,	0.0f,
			35.7584f,	71.5169f,	11.9195f,	0.0f,
			18.0481f,	7.2192f,	95.0532f,	0.0f,
			0.0f, 		0.0f,		0.0f,		1.0f
		};
	case ColorPrimaries::BT2020: 
		return {
			63.6958f,	26.2700f,	0.0f,		0.0f,
			14.4617f,	67.7998f,	2.8073f,	0.0f,
			16.8881f,	5.9302f,	106.0985f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		};
	case ColorPrimaries::SMPTE432: 
		return {
			48.6571f,	22.8975f,	0.0f,		0.0f,
			26.5668f,	69.1738f,	4.5113f,	0.0f,
			19.8217f,	7.9287f,	104.3945f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		};
	case ColorPrimaries::ADOBE_RGB: 
		return {
			57.6669f,	29.7345f,	2.7031f,	0.0f,
			18.5558f,	62.7364f,	7.0689f,	0.0f,
			18.8229f,	7.5291f,	99.1338f,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		};

	default: return {};
	}
}

	
}