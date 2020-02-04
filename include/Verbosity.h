#pragma once

#include "Macros.h"

namespace Zuazo {

enum class Verbosity {
	SILENT,
	ERROR,
	WARNING,
	INFO,
	VERBOSE
};

ZUAZO_ENUM_COMP_OPERATORS(Verbosity)

}