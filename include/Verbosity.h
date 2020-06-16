#pragma once

#include "Utils/Functions.h"

#include <string_view>

namespace Zuazo {

enum class Severity {
	ERROR		= Utils::bit(0),
	WARNING		= Utils::bit(1),
	INFO		= Utils::bit(2),
	VERBOSE		= Utils::bit(3),
};

enum class Verbosity {
	SILENT 		= 0,
	ERROR		= static_cast<int>(Severity::ERROR),
	WARNING		= static_cast<int>(Severity::WARNING),
	INFO		= static_cast<int>(Severity::INFO),
	VERBOSE		= static_cast<int>(Severity::VERBOSE),

	LE_ERROR	= SILENT,
	LEQ_ERROR	= LE_ERROR | ERROR,
	LE_WARINING	= LEQ_ERROR,
	LEQ_WARNING	= LE_WARINING | WARNING,
	LE_INFO		= LEQ_WARNING,
	LEQ_INFO	= LE_INFO | INFO,
	LE_VERBOSE	= LEQ_INFO,
	LEQ_VERBOSE	= LE_VERBOSE | VERBOSE,

	ALL			= LEQ_VERBOSE
};

ZUAZO_ENUM_BIT_OPERATORS(Verbosity)

constexpr Verbosity operator& (Verbosity a, Severity b);
constexpr Verbosity operator& (Severity a, Verbosity b);

}

namespace Zuazo {
	
constexpr std::string_view toString(Severity verb);

}

#include "Verbosity.inl"