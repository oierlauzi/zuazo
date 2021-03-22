#include "BlendingMode.h"

namespace Zuazo {

constexpr std::string_view toString(BlendingMode mode) noexcept {
	switch(mode){

	ZUAZO_ENUM2STR_CASE( BlendingMode, WRITE )
	ZUAZO_ENUM2STR_CASE( BlendingMode, OPACITY )
	ZUAZO_ENUM2STR_CASE( BlendingMode, ADD )
	ZUAZO_ENUM2STR_CASE( BlendingMode, DIFFERENCE )
	ZUAZO_ENUM2STR_CASE( BlendingMode, DIFFERENCE_1 )
	ZUAZO_ENUM2STR_CASE( BlendingMode, DARKEN )
	ZUAZO_ENUM2STR_CASE( BlendingMode, LIGHTEN )
	ZUAZO_ENUM2STR_CASE( BlendingMode, MULTIPLY )
	ZUAZO_ENUM2STR_CASE( BlendingMode, SCREEN )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, BlendingMode mode) {
	return os << toString(mode);
}

namespace Utils {

constexpr BlendingMode EnumTraits<BlendingMode>::first() noexcept {
	return BlendingMode::NONE + static_cast<BlendingMode>(1);
}

constexpr BlendingMode EnumTraits<BlendingMode>::last() noexcept {
	return BlendingMode::COUNT - static_cast<BlendingMode>(1);
}

}

}
