#include "ColorRange.h"

#include "StringConversions.h"

namespace Zuazo {

}

namespace Zuazo {

constexpr std::string_view toString(ColorRange colorRange) noexcept {
	switch(colorRange){

	ZUAZO_ENUM2STR_CASE( ColorRange, FULL )
	ZUAZO_ENUM2STR_CASE( ColorRange, ITU_NARROW )
	ZUAZO_ENUM2STR_CASE( ColorRange, ITU_NARROW_FULL_ALPHA )

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
	return ColorRange::NONE + static_cast<ColorRange>(1);
}

constexpr ColorRange EnumTraits<ColorRange>::last() noexcept {
	return ColorRange::COUNT - static_cast<ColorRange>(1);
}

}

}