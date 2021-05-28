#include "ColorRange.h"

#include "StringConversions.h"

namespace Zuazo {

}

namespace Zuazo {

constexpr std::string_view toString(ColorRange colorRange) noexcept {
	switch(colorRange){

	ZUAZO_ENUM2STR_CASE( ColorRange, full )
	ZUAZO_ENUM2STR_CASE( ColorRange, ituNarrow )
	ZUAZO_ENUM2STR_CASE( ColorRange, ituNarrowFullAlpha )

	default: return "";
	}
}

inline bool fromString(std::string_view str, ColorRange& range) {
	return enumFromString(str, range);
}

inline std::ostream& operator<<(std::ostream& os, ColorRange colorRange) {
	return os << toString(colorRange);
}


namespace Utils {

constexpr ColorRange EnumTraits<ColorRange>::first() noexcept {
	return ColorRange::none + static_cast<ColorRange>(1);
}

constexpr ColorRange EnumTraits<ColorRange>::last() noexcept {
	return ColorRange::count - static_cast<ColorRange>(1);
}

}

}