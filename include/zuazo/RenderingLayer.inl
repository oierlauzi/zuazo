#include "RenderingLayer.h"

namespace Zuazo {

constexpr std::string_view toString(RenderingLayer layer) noexcept {
	switch(layer){

	ZUAZO_ENUM2STR_CASE( RenderingLayer, BACKGROUND )
	ZUAZO_ENUM2STR_CASE( RenderingLayer, SCENE )
	ZUAZO_ENUM2STR_CASE( RenderingLayer, FOREGROUND )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, RenderingLayer layer) {
	return os << toString(layer);
}

namespace Utils {

constexpr RenderingLayer EnumTraits<RenderingLayer>::first() noexcept {
	return RenderingLayer::NONE + static_cast<RenderingLayer>(1);
}

constexpr RenderingLayer EnumTraits<RenderingLayer>::last() noexcept {
	return RenderingLayer::COUNT - static_cast<RenderingLayer>(1);
}

}

}