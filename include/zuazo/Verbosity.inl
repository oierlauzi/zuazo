#include "Verbosity.h"

#include "StringConversions.h"

namespace Zuazo {
	
constexpr std::string_view toString(Severity severity) noexcept {
	switch(severity) {
	ZUAZO_ENUM2STR_CASE( Severity, ERROR )
	ZUAZO_ENUM2STR_CASE( Severity, WARNING )
	ZUAZO_ENUM2STR_CASE( Severity, INFO )
	ZUAZO_ENUM2STR_CASE( Severity, VERBOSE )

	default: return {};
	}
}


inline std::ostream& operator<<(std::ostream& os, Severity severity) {
	return os << toString(severity);
}

}