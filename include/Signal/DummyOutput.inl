#include "DummyOutput.h"

namespace Zuazo::Signal {

template<typename T>
inline const T& DummyOutput<T>::get() const {
	return source.get();
}

}