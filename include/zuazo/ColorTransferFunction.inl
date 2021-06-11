#include "ColorTransferFunction.h"

#include "StringConversions.h"

namespace Zuazo {
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc) noexcept {
	switch(colorTransferFunc){

	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, linear )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, bt1886 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, gamma22 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, gamma28 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, iec61966_2_1 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, iec61966_2_4 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, smpte240M )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, smpte2084 )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, aribStdB67 )

	default: return "";
	}
}	

inline size_t fromString(std::string_view str, ColorTransferFunction& trf) {
	return enumFromString(str, trf);
}

inline std::ostream& operator<<(std::ostream& os, ColorTransferFunction colorTransferFunc) {
	return os << toString(colorTransferFunc);
}


namespace Utils {

constexpr ColorTransferFunction EnumTraits<ColorTransferFunction>::first() noexcept {
	return ColorTransferFunction::none + static_cast<ColorTransferFunction>(1);
}

constexpr ColorTransferFunction EnumTraits<ColorTransferFunction>::last() noexcept {
	return ColorTransferFunction::count - static_cast<ColorTransferFunction>(1);
}

}

}
