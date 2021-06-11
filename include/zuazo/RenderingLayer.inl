#include "RenderingLayer.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(RenderingLayer layer) noexcept {
	switch(layer){

	ZUAZO_ENUM2STR_CASE( RenderingLayer, background )
	ZUAZO_ENUM2STR_CASE( RenderingLayer, scene )
	ZUAZO_ENUM2STR_CASE( RenderingLayer, foreground )

	default: return "";
	}
}

inline size_t fromString(std::string_view str, RenderingLayer& layer) {
	return enumFromString(str, layer);
}

inline std::ostream& operator<<(std::ostream& os, RenderingLayer layer) {
	return os << toString(layer);
}

namespace Utils {

constexpr RenderingLayer EnumTraits<RenderingLayer>::first() noexcept {
	return RenderingLayer::none + static_cast<RenderingLayer>(1);
}

constexpr RenderingLayer EnumTraits<RenderingLayer>::last() noexcept {
	return RenderingLayer::count - static_cast<RenderingLayer>(1);
}

}

}