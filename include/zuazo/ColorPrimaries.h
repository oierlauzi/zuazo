#pragma once

#include "Macros.h"
#include "Math/Matrix.h"

#include <type_traits>
#include <string_view>
#include <ostream>

namespace Zuazo {

/**
 * ColorPrimaries represents which red, green and blue color primaries are used to 
 * sythetise color. This influences the range of representable range of colors
 */
enum class ColorPrimaries {
	NONE = 0,					///<Not supported

	BT601_625,					///<Used in PAL and SECAM \see https://www.itu.int/rec/R-REC-BT.601/en
	BT601_525,					///<Used in NTSC \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<Used in HDTV and sRGB color space\see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<Used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE431,					///<Used in DCI-P3 color space
	SMPTE432,					///<Used in Display P3 color space
	ADOBE_RGB,					///<Used in Adobe RGB color space

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorPrimaries)
ZUAZO_ENUM_COMP_OPERATORS(ColorPrimaries)	

class Chromaticities {
public:
	constexpr explicit Chromaticities(	Math::Vec2f red = Math::Vec2f(1.0f, 0.0f),
										Math::Vec2f green = Math::Vec2f(0.0f, 1.0f),
										Math::Vec2f blue = Math::Vec2f(0.0f, 0.0f),
										Math::Vec2f white = Math::Vec2f(1.0f/3.0f),
										float whiteLuminance = 100.0f) noexcept;
	constexpr Chromaticities(const Chromaticities& other) = default;
	~Chromaticities() = default;

	constexpr Chromaticities&		operator=(const Chromaticities& other) = default;

	constexpr bool					operator==(const Chromaticities& other) const noexcept;
	constexpr bool					operator!=(const Chromaticities& other) const noexcept;

	constexpr void					setRedPrimary(Math::Vec2f prim) noexcept;
	constexpr const Math::Vec2f&	getRedPrimary() const noexcept;

	constexpr void					setGreenPrimary(Math::Vec2f prim) noexcept;
	constexpr const Math::Vec2f&	getGreenPrimary() const noexcept;

	constexpr void					setBluePrimary(Math::Vec2f prim) noexcept;
	constexpr const Math::Vec2f&	getBluePrimary() const noexcept;

	constexpr void					setWhitePoint(Math::Vec2f wp) noexcept;
	constexpr const Math::Vec2f&	getWhitePoint() const noexcept;

	constexpr void					setWhiteLuminance(float luminance) noexcept;
	constexpr float					getWhiteLuminance() const noexcept;

	constexpr Math::Mat3x3f			calculateRGB2XYZConversionMatrix() const noexcept;
	constexpr Math::Mat3x3f			calculateXYZ2RGBConversionMatrix() const noexcept;

private:
	Math::Vec2f 					m_red;
	Math::Vec2f 					m_green;
	Math::Vec2f 					m_blue;
	Math::Vec2f 					m_white;
	float 							m_whiteLuminance;
};


constexpr Chromaticities getChromaticities(ColorPrimaries colorPrim) noexcept;


template<typename T>
constexpr Math::Mat3x3<T> constructRGB2XYZConversionMatrix(	const T red_x,	const T red_y,
															const T green_x,const T green_y,
															const T blue_x, const T blue_y,
															const T white_x,const T white_y,
															const T white_luminance = T(1) ) noexcept;




constexpr std::string_view toString(ColorPrimaries colorPrim) noexcept;
std::ostream& operator<<(std::ostream& os, ColorPrimaries colorPrim);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorPrimaries> {
	static constexpr ColorPrimaries first() noexcept;
	static constexpr ColorPrimaries last() noexcept;
};

}

}

#include "ColorPrimaries.inl"