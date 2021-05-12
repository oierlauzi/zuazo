#include "DepthStencilFormat.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(DepthStencilFormat format) noexcept {
	switch(format){

	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, D16 )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, X8D24_32 )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, D32f )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, S8 )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, D16S8 )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, D24S8 )
	ZUAZO_ENUM2STR_CASE( DepthStencilFormat, D32fS8 )

	default: return "";
	}
}

inline bool fromString(std::string_view str, DepthStencilFormat& format) {
	return enumFromString(str, format);
}

inline std::ostream& operator<<(std::ostream& os, DepthStencilFormat format) {
	return os << toString(format);
}

namespace Utils {

constexpr DepthStencilFormat EnumTraits<DepthStencilFormat>::first() noexcept {
	return DepthStencilFormat::NONE + static_cast<DepthStencilFormat>(1);
}

constexpr DepthStencilFormat EnumTraits<DepthStencilFormat>::last() noexcept {
	return DepthStencilFormat::COUNT - static_cast<DepthStencilFormat>(1);
}

}

}
