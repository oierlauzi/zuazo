#include "ScalingMode.h"

#include "StringConversions.h"
#include "Math/Comparisons.h"

namespace Zuazo {

template<typename T>
constexpr Math::Vec2<T> scale(const Math::Vec2<T>& srcSize, const Math::Vec2<T>& dstSize, ScalingMode mode) noexcept {
	auto scale = dstSize / srcSize; 

	switch(mode){
	case ScalingMode::box:
		scale = Math::Vec2<T>(Math::min(scale.x, scale.y));
		break;
	case ScalingMode::crop:
		scale = Math::Vec2<T>(Math::max(scale.x, scale.y));
		break;
	case ScalingMode::clampHorizontally:
		scale = Math::Vec2<T>(scale.x);
		break;
	case ScalingMode::clampVertically:
		scale = Math::Vec2<T>(scale.y);
		break;
	default: //ScalingMode::stretch
		break;
	}

	return srcSize * scale;
}

constexpr std::string_view toString(ScalingMode  mode) noexcept {
	switch(mode){

	ZUAZO_ENUM2STR_CASE( ScalingMode, stretch )
	ZUAZO_ENUM2STR_CASE( ScalingMode, box )
	ZUAZO_ENUM2STR_CASE( ScalingMode, crop )
	ZUAZO_ENUM2STR_CASE( ScalingMode, clampHorizontally )
	ZUAZO_ENUM2STR_CASE( ScalingMode, clampVertically )

	default: return "";
	}
}

inline size_t fromString(std::string_view str, ScalingMode& mode) {
	return enumFromString(str, mode);
}

inline std::ostream& operator<<(std::ostream& os, ScalingMode scalingMode) {
	return os << toString(scalingMode);
}


namespace Utils {

constexpr ScalingMode EnumTraits<ScalingMode>::first() noexcept {
	return ScalingMode::none + static_cast<ScalingMode>(1);
}

constexpr ScalingMode EnumTraits<ScalingMode>::last() noexcept {
	return ScalingMode::count - static_cast<ScalingMode>(1);
}

}

}