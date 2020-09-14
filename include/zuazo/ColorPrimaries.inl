#include "ColorPrimaries.h"

namespace Zuazo {

template<typename T>
constexpr Math::Mat4x4<T> constructRGB2XYZConversionMatrix(	const T white_x,const T white_y,
															const T red_x,	const T red_y,
															const T green_x,const T green_y,
															const T blue_x, const T blue_y,
															const T white_luminance )
{
	//Based on: 
	//https://pdfs.semanticscholar.org/a7a3/0558ee8036460b8570f34a4dd0ef6c8a7fb3.pdf

	//Define the rest of the coordenates provided that x+y+z=1
	const T	white_z = T(1) - white_x - white_y,
			red_z = T(1) - red_x - red_y,
			green_z = T(1) - green_x - green_y,
			blue_z = T(1) - blue_x - blue_y;

	//Obtain the white point
	const auto white = Math::Vec4<T>(
		white_luminance * white_x / white_y, 
		white_luminance,//white_y / white_y,
		white_luminance * white_z / white_y, 
		T(1)
	);

	//Express the primary chromaticities as a matrix
	//Note that it gets transposed as glm expects the matrix to be specified
	//in column major order, while it is being specified in row major
	const auto chromaticities = glm::transpose(
		Math::Mat4x4<T>(
			red_x, green_x, blue_x, T(0),
			red_y, green_y, blue_y, T(0),
			red_z, green_z, blue_z, T(0),
			T(0),  T(0),    T(0),   T(1)
		)
	);

	//As T * Vec4(1) == white, obtain the scaling factors
	const auto diagonal = glm::inverse(chromaticities) * white;
	const auto scale = Math::Mat4x4<T>(
		diagonal.r, T(0), 		T(0), 		T(0),
		T(0), 		diagonal.g, T(0), 		T(0),
		T(0), 		T(0), 		diagonal.b, T(0),
		T(0), 		T(0), 		T(0), 		diagonal.a
	);

	//Scale the chromaticities accordingly
	return chromaticities * scale;
}

constexpr Math::Mat4x4f getRGB2XYZConversionMatrix(ColorPrimaries colorPrim){
	switch(colorPrim){
	case ColorPrimaries::BT601_625: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/Rec._601
		0.3127f,	0.3290f,
		0.640f,		0.330f,
		0.290f, 	0.600f,
		0.150f, 	0.060f
	);

	case ColorPrimaries::BT601_525: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/Rec._601
		0.3127f,	0.3290f,
		0.630f, 	0.340f,
		0.310f, 	0.595f,
		0.155f, 	0.070f
	);

	case ColorPrimaries::BT709: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/Rec._709
		0.3127f, 	0.3290f,
		0.64f,		0.33f,
		0.30f,	 	0.60f,
		0.15f,	 	0.06f
	);

	case ColorPrimaries::BT2020: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/Rec._2020
		0.3127f, 	0.3290f,
		0.708f, 	0.292f,
		0.17f, 		0.797f,
		0.131f, 	0.046f
	);

	case ColorPrimaries::SMPTE431: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/DCI-P3
		0.314f,	 	0.351f,
		0.680f, 	0.320f,
		0.265f, 	0.690f,
		0.150f, 	0.060f
	);

	case ColorPrimaries::SMPTE432: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/DCI-P3 Just like the above one w/ different white point
		0.3127f, 	0.3290f,
		0.680f, 	0.320f,
		0.265f, 	0.690f,
		0.150f, 	0.060f
	);

	case ColorPrimaries::ADOBE_RGB: return constructRGB2XYZConversionMatrix(
		//https://en.wikipedia.org/wiki/Adobe_RGB_color_space
		0.3127f, 	0.3290f, 
		0.6400f, 	0.3300f, 
		0.2100f, 	0.7100f, 
		0.1500f, 	0.0600f
	);

	default: return {};
	}
}



constexpr std::string_view toString(ColorPrimaries colorPrim){
	switch(colorPrim){

	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT601_625 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT601_525 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, SMPTE431 )
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