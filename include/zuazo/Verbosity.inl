#include "Verbosity.h"

#include "StringConversions.h"

namespace Zuazo {
	
constexpr std::string_view toString(Severity severity) noexcept {
	switch(severity) {
	ZUAZO_ENUM2STR_CASE( Severity, error )
	ZUAZO_ENUM2STR_CASE( Severity, warning )
	ZUAZO_ENUM2STR_CASE( Severity, info )
	ZUAZO_ENUM2STR_CASE( Severity, verbose )

	default: return {};
	}
}


inline std::ostream& operator<<(std::ostream& os, Severity severity) {
	return os << toString(severity);
}

}