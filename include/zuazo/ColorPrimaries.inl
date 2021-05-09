#include "ColorPrimaries.h"

#include <tuple>

namespace Zuazo {

template<typename T>
constexpr Math::Mat3x3<T> constructRGB2XYZConversionMatrix(	const T red_x,	const T red_y,
															const T green_x,const T green_y,
															const T blue_x, const T blue_y,
															const T white_x,const T white_y,
															const T white_luminance ) noexcept
{
	//Based on: 
	//https://pdfs.semanticscholar.org/a7a3/0558ee8036460b8570f34a4dd0ef6c8a7fb3.pdf

	//Define the rest of the coordinates provided that x+y+z=1
	const T	red_z 	= T(1) - red_x - red_y,
			green_z = T(1) - green_x - green_y,
			blue_z 	= T(1) - blue_x - blue_y,
			white_z = T(1) - white_x - white_y;

	//Obtain the white point
	const auto white = Math::Vec3<T>(
		white_luminance * white_x / white_y, 
		white_luminance,//white_y / white_y,
		white_luminance * white_z / white_y
	);

	//Express the primary chromaticities as a matrix
	const Math::Mat3x3<T> chromaticities(
		red_x, green_x, blue_x,
		red_y, green_y, blue_y,
		red_z, green_z, blue_z
	);

	//As T * Vec4(1) == white, obtain the scaling factors
	const auto diagonal = transform(Math::inv(chromaticities), white);
	const auto scale = Math::Mat3x3<T>(diagonal.x, diagonal.y, diagonal.z);

	//Compute the tristimulus values scaling them by the scaling factor
	return chromaticities * scale;
}



constexpr Chromaticities::Chromaticities(	Math::Vec2f red,
											Math::Vec2f green,
											Math::Vec2f blue,
											Math::Vec2f white,
											float whiteLuminance ) noexcept
	: m_red(red)
	, m_green(green)
	, m_blue(blue)
	, m_white(white)
	, m_whiteLuminance(whiteLuminance)
{
}


constexpr bool Chromaticities::operator==(const Chromaticities& other) const noexcept {
	return std::tie(m_red, m_green, m_blue, m_white, m_whiteLuminance) == std::tie(other.m_red, other.m_green, other.m_blue, other.m_white, other.m_whiteLuminance);
}

constexpr bool Chromaticities::operator!=(const Chromaticities& other) const noexcept {
	return !operator==(other);
}


constexpr void Chromaticities::setRedPrimary(Math::Vec2f prim) noexcept {
	m_red = prim;
}

constexpr const Math::Vec2f& Chromaticities::getRedPrimary() const noexcept {
	return m_red;
}


constexpr void Chromaticities::setGreenPrimary(Math::Vec2f prim) noexcept {
	m_green = prim;
}

constexpr const Math::Vec2f& Chromaticities::getGreenPrimary() const noexcept {
	return m_green;
}


constexpr void Chromaticities::setBluePrimary(Math::Vec2f prim) noexcept {
	m_blue = prim;
}

constexpr const Math::Vec2f& Chromaticities::getBluePrimary() const noexcept {
	return m_blue;
}


constexpr void Chromaticities::setWhitePoint(Math::Vec2f wp) noexcept {
	m_white = wp;
}

constexpr const Math::Vec2f& Chromaticities::getWhitePoint() const noexcept {
	return m_white;
}


constexpr void Chromaticities::setWhiteLuminance(float luminance) noexcept {
	m_whiteLuminance = luminance;
}

constexpr float Chromaticities::getWhiteLuminance() const noexcept {
	return m_whiteLuminance;
}


constexpr Math::Mat3x3f Chromaticities::calculateRGB2XYZConversionMatrix() const noexcept {
	return constructRGB2XYZConversionMatrix(
		getRedPrimary().x, getRedPrimary().y,
		getGreenPrimary().x, getGreenPrimary().y,
		getBluePrimary().x, getBluePrimary().y,
		getWhitePoint().x, getWhitePoint().y,
		getWhiteLuminance()
	);
}
constexpr Math::Mat3x3f Chromaticities::calculateXYZ2RGBConversionMatrix() const noexcept {
	return Math::inv(calculateRGB2XYZConversionMatrix());
}



constexpr Chromaticities getChromaticities(ColorPrimaries colorPrim) noexcept {
	switch(colorPrim){
	case ColorPrimaries::BT601_625: return Chromaticities(
		//https://en.wikipedia.org/wiki/Rec._601
		Math::Vec2f(0.640f,		0.330f),
		Math::Vec2f(0.290f, 	0.600f),
		Math::Vec2f(0.150f, 	0.060f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		100.0f
	);

	case ColorPrimaries::BT601_525: return Chromaticities(
		//https://en.wikipedia.org/wiki/Rec._601
		Math::Vec2f(0.630f, 	0.340f),
		Math::Vec2f(0.310f, 	0.595f),
		Math::Vec2f(0.155f, 	0.070f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		100.0f
	);

	case ColorPrimaries::BT709: return Chromaticities(
		//https://en.wikipedia.org/wiki/Rec._709
		Math::Vec2f(0.64f,		0.33f),
		Math::Vec2f(0.30f,	 	0.60f),
		Math::Vec2f(0.15f,	 	0.06f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		100.0f
	);

	case ColorPrimaries::BT2020: return Chromaticities(
		//https://en.wikipedia.org/wiki/Rec._2020
		Math::Vec2f(0.708f, 	0.292f),
		Math::Vec2f(0.17f, 		0.797f),
		Math::Vec2f(0.131f, 	0.046f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		100.0f //Actually not specified.
	);

	case ColorPrimaries::SMPTE431: return Chromaticities(
		//https://en.wikipedia.org/wiki/DCI-P3
		Math::Vec2f(0.680f, 	0.320f),
		Math::Vec2f(0.265f, 	0.690f),
		Math::Vec2f(0.150f, 	0.060f),
		Math::Vec2f(0.314f, 	0.351f), //D50
		48.0f
	);

	case ColorPrimaries::SMPTE432: return Chromaticities(
		//https://en.wikipedia.org/wiki/DCI-P3 Just like the above one w/ different white point
		Math::Vec2f(0.680f, 	0.320f),
		Math::Vec2f(0.265f, 	0.690f),
		Math::Vec2f(0.150f, 	0.060f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		48.0f
	);

	case ColorPrimaries::IEC61966_2_1: return Chromaticities(
		//https://en.wikipedia.org/wiki/Rec._709
		Math::Vec2f(0.64f,		0.33f),
		Math::Vec2f(0.30f,	 	0.60f),
		Math::Vec2f(0.15f,	 	0.06f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		80.0f
	);

	case ColorPrimaries::ADOBE_RGB: return Chromaticities(
		//https://en.wikipedia.org/wiki/Adobe_RGB_color_space
		Math::Vec2f(0.6400f, 	0.3300f), 
		Math::Vec2f(0.2100f, 	0.7100f), 
		Math::Vec2f(0.1500f, 	0.0600f),
		Math::Vec2f(0.3127f,	0.3290f), //D65
		160.0f
	);

	default: return Chromaticities();
	}
}



constexpr std::string_view toString(ColorPrimaries colorPrim) noexcept {
	switch(colorPrim){

	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT601_625 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT601_525 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT709 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, BT2020 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, SMPTE431 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, SMPTE432 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, IEC61966_2_1 )
	ZUAZO_ENUM2STR_CASE( ColorPrimaries, ADOBE_RGB )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ColorPrimaries colorPrim) {
	return os << toString(colorPrim);
}


namespace Utils {

constexpr ColorPrimaries EnumTraits<ColorPrimaries>::first() noexcept {
	return ColorPrimaries::NONE + static_cast<ColorPrimaries>(1);
}

constexpr ColorPrimaries EnumTraits<ColorPrimaries>::last() noexcept {
	return ColorPrimaries::COUNT - static_cast<ColorPrimaries>(1);
}

}

}