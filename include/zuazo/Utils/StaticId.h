#pragma once

#include <cstddef>

namespace Zuazo::Utils {

struct StaticId {
	uintptr_t get() const;
	operator uintptr_t() const;
};

}

#include "StaticId.inl"