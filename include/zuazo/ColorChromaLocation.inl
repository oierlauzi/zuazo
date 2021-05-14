#include "ColorChromaLocation.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(ColorChromaLocation chromaLoc) noexcept {
	switch(chromaLoc){

	ZUAZO_ENUM2STR_CASE( ColorChromaLocation, COSITED_0 )
	ZUAZO_ENUM2STR_CASE( ColorChromaLocation, MIDPOINT )

	default: return "";
	}
}

inline bool fromString(std::string_view str, ColorChromaLocation& location) {
	return enumFromString(str, location);
}

inline std::ostream& operator<<(std::ostream& os, ColorChromaLocation location) {
	return os << toString(location);
}


namespace Utils {

constexpr ColorChromaLocation EnumTraits<ColorChromaLocation>::first() noexcept {
	return ColorChromaLocation::NONE + static_cast<ColorChromaLocation>(1);
}

constexpr ColorChromaLocation EnumTraits<ColorChromaLocation>::last() noexcept {
	return ColorChromaLocation::COUNT - static_cast<ColorChromaLocation>(1);
}

}

}