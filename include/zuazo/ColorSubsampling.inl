#include "ColorSubsampling.h"

namespace Zuazo {

constexpr Resolution getSubsampledResolution(ColorSubsampling subs, Resolution res){
	switch(subs){

	case ColorSubsampling::RB_444: 
		return res;
	case ColorSubsampling::RB_440:
		return Resolution(res.width, res.height / 2);
	case ColorSubsampling::RB_422: 
		return Resolution(res.width / 2, res.height);
	case ColorSubsampling::RB_420:
		return Resolution(res.width / 2, res.height / 2);
	case ColorSubsampling::RB_411:
		return Resolution(res.width / 4, res.height);
	case ColorSubsampling::RB_410:
		return Resolution(res.width / 4, res.height / 2);
	case ColorSubsampling::RB_311: 
		return Resolution(res.width / 3, res.height);

	default: return Resolution(0, 0);
	}
}



constexpr std::string_view toString(ColorSubsampling colorSubsampling){
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

inline std::ostream& operator<<(std::ostream& os, ColorSubsampling colorSubsampling) {
	return os << toString(colorSubsampling);
}


namespace Utils {

constexpr ColorSubsampling lowest(const Any<ColorSubsampling>&) {
	return ColorSubsampling::NONE + static_cast<ColorSubsampling>(1);
}

constexpr ColorSubsampling highest(const Any<ColorSubsampling>&) {
	return ColorSubsampling::COUNT - static_cast<ColorSubsampling>(1);
}

}

}