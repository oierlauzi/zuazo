#include "BlendingMode.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(BlendingMode mode) noexcept {
	switch(mode){

	ZUAZO_ENUM2STR_CASE( BlendingMode, write )
	ZUAZO_ENUM2STR_CASE( BlendingMode, opacity )
	ZUAZO_ENUM2STR_CASE( BlendingMode, add )
	ZUAZO_ENUM2STR_CASE( BlendingMode, differenceInv )
	ZUAZO_ENUM2STR_CASE( BlendingMode, difference )
	ZUAZO_ENUM2STR_CASE( BlendingMode, darken )
	ZUAZO_ENUM2STR_CASE( BlendingMode, lighten )
	ZUAZO_ENUM2STR_CASE( BlendingMode, multiply )
	ZUAZO_ENUM2STR_CASE( BlendingMode, screen )

	default: return "";
	}
}

inline bool fromString(std::string_view str, BlendingMode& mode) {
	return enumFromString(str, mode);
}

inline std::ostream& operator<<(std::ostream& os, BlendingMode mode) {
	return os << toString(mode);
}

namespace Utils {

constexpr BlendingMode EnumTraits<BlendingMode>::first() noexcept {
	return BlendingMode::none + static_cast<BlendingMode>(1);
}

constexpr BlendingMode EnumTraits<BlendingMode>::last() noexcept {
	return BlendingMode::count - static_cast<BlendingMode>(1);
}

}

}
