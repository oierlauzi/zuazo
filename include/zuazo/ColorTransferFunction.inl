#include "ColorTransferFunction.h"

namespace Zuazo {
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc) noexcept {
	switch(colorTransferFunc){

	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, LINEAR )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, BT1886 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, GAMMA22 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, GAMMA28 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, IEC61966_2_1 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, IEC61966_2_4 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, SMPTE240M )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, SMPTE2084 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, ARIB_STD_B67 )

	default: return "";
	}
}	

inline std::ostream& operator<<(std::ostream& os, ColorTransferFunction colorTransferFunc) {
	return os << toString(colorTransferFunc);
}


namespace Utils {

constexpr ColorTransferFunction EnumTraits<ColorTransferFunction>::first() noexcept {
	return ColorTransferFunction::NONE + static_cast<ColorTransferFunction>(1);
}

constexpr ColorTransferFunction EnumTraits<ColorTransferFunction>::last() noexcept {
	return ColorTransferFunction::COUNT - static_cast<ColorTransferFunction>(1);
}

}

}
