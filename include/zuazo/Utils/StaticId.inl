#include "StaticId.h"

namespace Zuazo::Utils {

inline uintptr_t StaticId::get() const noexcept {
	return reinterpret_cast<uintptr_t>(this);
}

inline StaticId::operator uintptr_t() const noexcept {
	return get();
}

}