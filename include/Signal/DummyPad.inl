#include "DummyPad.h"

namespace Zuazo::Signal {

template <typename T>
DummyPad<T>::DummyPad(PadBase::Direction dir, std::string&& name) :
    PadBase(typeid(T), dir, std::forward<std::string>(name))
{
}

template <typename T>
inline const std::shared_ptr<const T>& DummyPad<T>::get() const{
    return InputPad<T>::get();
}



template <typename T>
inline DummyInput<T>::DummyInput(std::string&& name) :
    DummyPad<T>( std::forward<std::string>(name), PadBase::Direction::INPUT)
{
}



template <typename T>
inline DummyOutput<T>::DummyOutput(std::string&& name) :
    DummyPad<T>( std::forward<std::string>(name), PadBase::Direction::OUTPUT)
{
}

}