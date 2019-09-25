#include "DummyPad.h"

namespace Zuazo::Signal {

template <typename T>
DummyPad<T>::DummyPad(std::string&& name, PadBase::Direction dir) :
    PadBase(std::forward<std::string>(name), dir, typeid(T))
{
}

template <typename T>
inline const std::shared_ptr<const T>& DummyPad<T>::get() const{
    return InputPad<T>::get();
}


template <typename T>
inline DummyInput<T>::DummyInput(std::string&& name) :
    DummyPad<T>(PadBase::INPUT, std::forward<std::string>(name))
{
}

template <typename T>
inline DummyOutput<T>::DummyOutput(std::string&& name) :
    DummyPad<T>(PadBase::OUTPUT, std::forward<std::string>(name))
{
}

}