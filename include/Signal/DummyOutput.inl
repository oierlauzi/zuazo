#include "DummyOutput.h"

namespace Zuazo::Signal {

template<typename T>
inline const std::shared_ptr<const T>& DummyOutput<T>::get() const {
	return source.get();
}

}