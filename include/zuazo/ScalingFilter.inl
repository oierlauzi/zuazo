#include "ScalingFilter.h"

namespace Zuazo {

constexpr std::string_view toString(ScalingFilter filter) noexcept {
	switch(filter){

	ZUAZO_ENUM2STR_CASE( ScalingFilter, NEAREST )
	ZUAZO_ENUM2STR_CASE( ScalingFilter, LINEAR )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ScalingFilter filter) {
	return os << toString(filter);
}

namespace Utils {

constexpr ScalingFilter EnumTraits<ScalingFilter>::first() noexcept {
	return ScalingFilter::NONE + static_cast<ScalingFilter>(1);
}

constexpr ScalingFilter EnumTraits<ScalingFilter>::last() noexcept {
	return ScalingFilter::COUNT - static_cast<ScalingFilter>(1);
}

}

}