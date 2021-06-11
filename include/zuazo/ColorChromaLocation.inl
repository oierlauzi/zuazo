#include "ColorChromaLocation.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(ColorChromaLocation chromaLoc) noexcept {
	switch(chromaLoc){

	ZUAZO_ENUM2STR_CASE( ColorChromaLocation, cosited0 )
	ZUAZO_ENUM2STR_CASE( ColorChromaLocation, midpoint )

	default: return "";
	}
}

inline size_t fromString(std::string_view str, ColorChromaLocation& location) {
	return enumFromString(str, location);
}

inline std::ostream& operator<<(std::ostream& os, ColorChromaLocation location) {
	return os << toString(location);
}


namespace Utils {

constexpr ColorChromaLocation EnumTraits<ColorChromaLocation>::first() noexcept {
	return ColorChromaLocation::none + static_cast<ColorChromaLocation>(1);
}

constexpr ColorChromaLocation EnumTraits<ColorChromaLocation>::last() noexcept {
	return ColorChromaLocation::count - static_cast<ColorChromaLocation>(1);
}

}

}