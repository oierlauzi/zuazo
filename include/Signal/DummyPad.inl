#include "DummyPad.h"

namespace Zuazo::Signal {

template <typename T>
DummyPad<T>::DummyPad(std::string&& name) :
    PadBase(std::forward<std::string>(name))
{
}

template <typename T>
inline const std::shared_ptr<const T>& DummyPad<T>::get() const{
    return ConsumerPad<T>::get();
}

}