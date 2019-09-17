#include "Consumer.h"

namespace Zuazo::Stream {

template <typename T>
Consumer<T>::Consumer(){

}

template <typename T>
Consumer<T>::Consumer(const Consumer& other){

}

template <typename T>
Consumer<T>::Consumer(Consumer&& other){

}

template <typename T>
Consumer<T>::~Consumer(){

}

template <typename T>
void Consumer<T>::setSource(Source<T>& src){
    setSource(&src);
}

template <typename T>
void Consumer<T>::setSource(Source<T>* src){
    if(m_source){
        //There was a source previously set
        m_source->m_consumers.erase(this);
    }

    m_source = src; //Assing the new source

    if(m_source){
        //It is a valid source. Set it active
        m_source->m_consumers.insert(this);
    }
}

template <typename T>
const std::shared_ptr<const T>& Consumer<T>::get() const{
    if(m_source) {
        m_lastElement = m_source->get();
    } else {
        m_lastElement = nullptr;
    }

    return m_lastElement;
}

template <typename T>
bool Consumer<T>::hasChanged() const{
    if(m_source){
        return m_lastElement == m_source->get();
    }else {
        return m_lastElement == nullptr;
    }
}
}