#pragma once

#include "Utils/Bit.h"

#include <string_view>

namespace Zuazo {

enum class Severity {
	verbose		= Utils::bit(0),
	info		= Utils::bit(1),
	warning		= Utils::bit(2),
	error		= Utils::bit(3),
};

enum class Verbosity {
	silent 		= 0,
	error		= static_cast<int>(Severity::error),
	warning		= static_cast<int>(Severity::warning),
	info		= static_cast<int>(Severity::info),
	verbose		= static_cast<int>(Severity::verbose),

	geError		= silent,
	geqError	= geError | error,
	geWarning	= geqError,
	geqWarning	= geWarning | warning,
	geInfo		= geqWarning,
	geqInfo		= geInfo | info,
	geVerbose	= geqInfo,
	geqVerbose	= geVerbose | verbose,

	all			= geqVerbose
};

ZUAZO_ENUM_COMP_OPERATORS(Verbosity)
ZUAZO_ENUM_COMP_OPERATORS2(Verbosity, Severity)
ZUAZO_ENUM_COMP_OPERATORS2(Severity, Verbosity)
ZUAZO_ENUM_BIT_OPERATORS(Verbosity)
ZUAZO_ENUM_BIT_OPERATORS2(Verbosity, Severity)

	
constexpr std::string_view toString(Severity severity) noexcept;
std::ostream& operator<<(std::ostream& os, Severity severity);

}

#include "Verbosity.inl"
