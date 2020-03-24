#pragma once

#include <cstddef>

namespace Zuazo::Utils {

struct StaticId {
	size_t get() const;
	operator size_t() const;
};

}

#include "StaticId.inl"