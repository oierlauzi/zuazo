#include "OutputPad.h"

#include "InputPad.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline OutputPad<T>::OutputPad(std::string&& name) : 
    PadBase(std::forward<std::string>(name), PadBase::OUTPUT, typeid(T))
{
}

template <typename T>
inline OutputPad<T>::~OutputPad(){
    const auto myConsumers = m_consumers;  //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        cons->setSource(nullptr);
    }
}

template <typename T>
inline const std::set<InputPad<T>*>& OutputPad<T>::getConsumers() const{
    return m_consumers;
}

}
