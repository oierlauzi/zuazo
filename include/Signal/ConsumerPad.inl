#include "ConsumerPad.h"

#include "SourcePad.h"

namespace Zuazo::Signal {

template <typename T>
const std::shared_ptr<const T> ConsumerPad<T>::NO_SIGNAL;

template <typename T>
inline ConsumerPad<T>::ConsumerPad(std::string&& name) : 
    PadBase(std::forward<std::string>(name))
{
}

template <typename T>
inline ConsumerPad<T>::ConsumerPad(const ConsumerPad& other){
    setSource(other.m_source);
}

template <typename T>
inline ConsumerPad<T>::ConsumerPad(ConsumerPad&& other){
    setSource(other.m_source);
    other.setSource(nullptr);
}

template <typename T>
inline ConsumerPad<T>::~ConsumerPad(){
    setSource(nullptr);
}

template <typename T>
inline void ConsumerPad<T>::setSource(SourcePad<T>* src){
    if(m_source){
        //There was a source previously set
        m_source->m_consumers.erase(this);
    }

    m_source = src; //Assing the new source

    if(m_source){
        //A new souce has been assigned
        m_source->m_consumers.insert(this);
    }
}

template <typename T>
inline SourcePad<T>* ConsumerPad<T>::getSource() const{
    return m_source;
}

template <typename T>
inline const std::shared_ptr<const T>& ConsumerPad<T>::get() const{
    if(m_source) {
        return m_source->get();
    } else {
        return NO_SIGNAL;
    }
}

}