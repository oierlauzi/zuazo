#include "OutputPad.h"

#include "InputPad.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline OutputPad<T>::OutputPad(std::string&& name) : 
    PadBase(typeid(T), PadBase::Direction::OUTPUT, std::forward<std::string>(name))
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

template <typename T>
OutputPad<T>::OutputPad(OutputPad&& other) :
    PadBase(other)
{
    //Steal it's consumers
    const auto myConsumers = other.m_consumers;  //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        cons->setSource(this);
    }
}

template <typename T>
OutputPad<T>& OutputPad<T>::operator=(OutputPad&& other){
    static_cast<PadBase&>(*this) = other;

    //Unset all my consumers
    auto myConsumers = m_consumers; //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        cons->setSource(nullptr);
    }

    //Steal it's consumers
    myConsumers = other.m_consumers;  //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        cons->setSource(this);
    }

}

}
