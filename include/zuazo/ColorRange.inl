#include "ColorRange.h"

namespace Zuazo {

}

namespace Zuazo {

constexpr std::string_view toString(ColorRange colorRange){
	switch(colorRange){

	ZUAZO_ENUM2STR_CASE( ColorRange, FULL_RGB )
	ZUAZO_ENUM2STR_CASE( ColorRange, FULL_YCBCR )
	ZUAZO_ENUM2STR_CASE( ColorRange, ITU_NARROW_RGB )
	ZUAZO_ENUM2STR_CASE( ColorRange, ITU_NARROW_YCBCR )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ColorRange colorRange) {
	return os << toString(colorRange);
}


namespace Utils {

constexpr ColorRange lowest(const Any<ColorRange>&) {
	return ColorRange::NONE + static_cast<ColorRange>(1);
}

constexpr ColorRange highest(const Any<ColorRange>&) {
	return ColorRange::COUNT - static_cast<ColorRange>(1);
}

}

}