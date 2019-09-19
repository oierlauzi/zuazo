#include "SourcePad.h"

#include "ConsumerPad.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline SourcePad<T>::SourcePad(std::string&& name) : 
    PadBase(std::forward<std::string>(name))
{
}

template <typename T>
inline SourcePad<T>::SourcePad(const SourcePad& other) : SourcePad()
{
}

template <typename T>
inline SourcePad<T>::SourcePad(SourcePad&& other) {
    const auto myConsumers = other.m_consumers; //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        //Steals consumers from the other
        cons->setSource(this);
    }
}

template <typename T>
inline SourcePad<T>::~SourcePad(){
    const auto myConsumers = m_consumers;  //Copy the set, as it will be modified
    for(auto cons : myConsumers){
        cons->setSource(nullptr);
    }
}

template <typename T>
inline const std::set<ConsumerPad<T>*>& SourcePad<T>::getConsumers() const{
    return m_consumers;
}

}
