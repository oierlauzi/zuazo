#include "ColorSubsampling.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr Math::Vec2i getSubsamplingFactor(ColorSubsampling subs) noexcept {
	switch(subs){

	case ColorSubsampling::RB_444: 	return Math::Vec2i(1, 1);
	case ColorSubsampling::RB_440:  return Math::Vec2i(1, 2);
	case ColorSubsampling::RB_422:	return Math::Vec2i(2, 1);
	case ColorSubsampling::RB_420:	return Math::Vec2i(2, 2);
	case ColorSubsampling::RB_411:	return Math::Vec2i(4, 1);
	case ColorSubsampling::RB_410:	return Math::Vec2i(4, 2);
	case ColorSubsampling::RB_311:	return Math::Vec2i(3, 1);
	default: 						return Math::Vec2i(0, 0);
	}
}

constexpr Resolution getSubsampledResolution(ColorSubsampling subs, Resolution res) noexcept {
	const auto factor = Resolution(getSubsamplingFactor(subs));
	return factor ? res / factor : Resolution(0, 0);
}



constexpr std::string_view toString(ColorSubsampling colorSubsampling) noexcept {
	switch(colorSubsampling){

	case ColorSubsampling::RB_444: return "4:4:4";
	case ColorSubsampling::RB_440: return "4:4:0";
	case ColorSubsampling::RB_422: return "4:2:2";
	case ColorSubsampling::RB_420: return "4:2:0";
	case ColorSubsampling::RB_411: return "4:1:1";
	case ColorSubsampling::RB_410: return "4:1:0";
	case ColorSubsampling::RB_311: return "3:1:1";

	default: return "";
	}
}

inline bool fromString(std::string_view str, ColorSubsampling& subsampling) {
	return enumFromString(str, subsampling);
}

inline std::ostream& operator<<(std::ostream& os, ColorSubsampling colorSubsampling) {
	return os << toString(colorSubsampling);
}


namespace Utils {

constexpr ColorSubsampling EnumTraits<ColorSubsampling>::first() noexcept {
	return ColorSubsampling::NONE + static_cast<ColorSubsampling>(1);
}

constexpr ColorSubsampling EnumTraits<ColorSubsampling>::last() noexcept {
	return ColorSubsampling::COUNT - static_cast<ColorSubsampling>(1);
}

}

}