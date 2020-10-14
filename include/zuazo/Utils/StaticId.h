#pragma once

#include <cstddef>

namespace Zuazo::Utils {

struct StaticId {
	uintptr_t get() const noexcept;
	operator uintptr_t() const noexcept;
};

}

#include "StaticId.inl"