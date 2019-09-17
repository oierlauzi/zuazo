#include "Source.h"

#include <utility>

namespace Zuazo::Stream {

template <typename T>
inline Source<T>::Source(const Source& other) : 
     m_lastElement(other.m_lastElement)
{
}


template <typename T>
inline Source<T>::Source(Source&& other) :
    m_lastElement(std::move(other.m_lastElement))
{
    const auto myConsumers = other.m_consumers;
    for(auto cons : myConsumers){
        //Steals consumers from the other
        addConsumer(cons);
    }
}

template <typename T>
inline Source<T>::~Source(){
    const auto myConsumers = m_consumers;
    for(auto cons : myConsumers){
        removeConsumer(cons);
    }
}

template <typename T>
inline void Source<T>::addConsumer(Consumer<T>& consumer){
    addConsumer(&consumer);
}

template <typename T>
inline void Source<T>::addConsumer(Consumer<T>* consumer){
    if(consumer) consumer->setSource(this);
}

template <typename T>
inline void Source<T>::removeConsumer(Consumer<T>& consumer){
    addConsumer(&consumer);
}

template <typename T>
inline void Source<T>::removeConsumer(Consumer<T>* consumer){
    if(consumer) consumer->setSource(nullptr);
}

template <typename T>
inline const std::set<Consumer<T>*>& Source<T>::getConsumers() const{
    return m_consumers;
}

template <typename T>
inline const std::shared_ptr<const T>& Source<T>::get() const{
    return m_lastElement;
}

template <typename T>
inline void Source<T>::reset(){
    m_lastElement.reset();
}

template <typename T>
inline void Source<T>::push(std::shared_ptr<const T>&& element){
    m_lastElement = std::forward<std::shared_ptr<const T>>(element);
}

}
