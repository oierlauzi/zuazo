#include "ColorTransferFunction.h"

namespace Zuazo {
	
constexpr std::string_view toString(ColorTransferFunction colorTransferFunc) {
	switch(colorTransferFunc){

	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, LINEAR )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, IEC61966_2_1 )

	default: return "";
	}
}	

inline std::ostream& operator<<(std::ostream& os, ColorTransferFunction colorTransferFunc) {
	return os << toString(colorTransferFunc);
}


namespace Utils {

constexpr ColorTransferFunction lowest(const Any<ColorTransferFunction>&) {
	return ColorTransferFunction::NONE + static_cast<ColorTransferFunction>(1);
}

constexpr ColorTransferFunction highest(const Any<ColorTransferFunction>&) {
	return ColorTransferFunction::COUNT - static_cast<ColorTransferFunction>(1);
}

}

}
