#include "ScalingMode.h"

#include "Math/Functions.h"

namespace Zuazo {

template<typename T>
constexpr Math::Vec2<T> scale(const Math::Vec2<T>& srcSize, const Math::Vec2<T>& dstSize, ScalingMode mode) {
	auto scale = dstSize / srcSize; 

	switch(mode){
	case ScalingMode::BOXED:
		scale = Math::Vec2<T>(Math::min(scale.x, scale.y));
		break;
	case ScalingMode::CROPPED:
		scale = Math::Vec2<T>(Math::max(scale.x, scale.y));
		break;
	case ScalingMode::CLAMP_HORIZONTALLY:
		scale = Math::Vec2<T>(scale.x);
		break;
	case ScalingMode::CLAMP_VERTICALLY:
		scale = Math::Vec2<T>(scale.y);
		break;
	default: //ScalingMode::STRETCH
		break;
	}

	return srcSize * scale;
}

constexpr std::string_view toString(ScalingMode  mode){
	switch(mode){

	ZUAZO_ENUM2STR_CASE( ScalingMode, STRETCH )
	ZUAZO_ENUM2STR_CASE( ScalingMode, BOXED )
	ZUAZO_ENUM2STR_CASE( ScalingMode, CROPPED )
	ZUAZO_ENUM2STR_CASE( ScalingMode, CLAMP_HORIZONTALLY )
	ZUAZO_ENUM2STR_CASE( ScalingMode, CLAMP_VERTICALLY )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ScalingMode scalingMode) {
	return os << toString(scalingMode);
}


namespace Utils {

constexpr ScalingMode lowest(const Any<ScalingMode>&) {
	return ScalingMode::NONE + static_cast<ScalingMode>(1);
}

constexpr ScalingMode highest(const Any<ScalingMode>&) {
	return ScalingMode::COUNT - static_cast<ScalingMode>(1);
}

}

}