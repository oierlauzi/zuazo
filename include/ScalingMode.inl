#include "ScalingMode.h"

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

}