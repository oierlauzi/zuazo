#include "ScalingFilter.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(ScalingFilter filter) noexcept {
	switch(filter){

	ZUAZO_ENUM2STR_CASE( ScalingFilter, nearest )
	ZUAZO_ENUM2STR_CASE( ScalingFilter, linear )
	ZUAZO_ENUM2STR_CASE( ScalingFilter, cubic )

	default: return "";
	}
}

inline size_t fromString(std::string_view str, ScalingFilter& filter) {
	return enumFromString(str, filter);
}

inline std::ostream& operator<<(std::ostream& os, ScalingFilter filter) {
	return os << toString(filter);
}

namespace Utils {

constexpr ScalingFilter EnumTraits<ScalingFilter>::first() noexcept {
	return ScalingFilter::none + static_cast<ScalingFilter>(1);
}

constexpr ScalingFilter EnumTraits<ScalingFilter>::last() noexcept {
	return ScalingFilter::count - static_cast<ScalingFilter>(1);
}

}

}