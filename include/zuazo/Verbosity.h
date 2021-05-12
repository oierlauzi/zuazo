#pragma once

#include "Utils/Bit.h"

#include <string_view>

namespace Zuazo {

enum class Severity {
	VERBOSE		= Utils::bit(0),
	INFO		= Utils::bit(1),
	WARNING		= Utils::bit(2),
	ERROR		= Utils::bit(3),
};

enum class Verbosity {
	SILENT 		= 0,
	ERROR		= static_cast<int>(Severity::ERROR),
	WARNING		= static_cast<int>(Severity::WARNING),
	INFO		= static_cast<int>(Severity::INFO),
	VERBOSE		= static_cast<int>(Severity::VERBOSE),

	GE_ERROR	= SILENT,
	GEQ_ERROR	= GE_ERROR | ERROR,
	GE_WARINING	= GEQ_ERROR,
	GEQ_WARNING	= GE_WARINING | WARNING,
	GE_INFO		= GEQ_WARNING,
	GEQ_INFO	= GE_INFO | INFO,
	GE_VERBOSE	= GEQ_INFO,
	GEQ_VERBOSE	= GE_VERBOSE | VERBOSE,

	ALL			= GEQ_VERBOSE
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