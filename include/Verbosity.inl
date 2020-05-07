#include "Verbosity.h"

namespace Zuazo {

constexpr Verbosity operator& (Verbosity a, Severity b) {
	using UnderVerb = std::underlying_type<Verbosity>::type;
	using UnderSev = std::underlying_type<Severity>::type;
	return static_cast<Verbosity>( static_cast<UnderVerb>(a) & static_cast<UnderSev>(b) );
}

constexpr Verbosity operator& (Severity a, Verbosity b) {
	using UnderVerb = std::underlying_type<Verbosity>::type;
	using UnderSev = std::underlying_type<Severity>::type;
	return static_cast<Verbosity>( static_cast<UnderSev>(a) & static_cast<UnderVerb>(b) );
}

}

namespace Zuazo::Utils {
	
constexpr std::string_view toString(Severity verb) {
	switch(verb) {
	ZUAZO_ENUM2STR_CASE( Severity, ERROR )
	ZUAZO_ENUM2STR_CASE( Severity, WARNING )
	ZUAZO_ENUM2STR_CASE( Severity, INFO )
	ZUAZO_ENUM2STR_CASE( Severity, VERBOSE )

	default: return {};
	}
}

}