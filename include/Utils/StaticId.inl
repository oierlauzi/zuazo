#include "StaticId.h"

namespace Zuazo::Utils {

inline size_t StaticId::get() const {
	return reinterpret_cast<uintptr_t>(this);
}

inline StaticId::operator size_t() const {
	return get();
}

}