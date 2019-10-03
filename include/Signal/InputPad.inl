#include "InputPad.h"

#include "OutputPad.h"

namespace Zuazo::Signal {

template <typename T>
inline const std::shared_ptr<const T> InputPad<T>::NO_SIGNAL;

template <typename T>
inline InputPad<T>::InputPad(std::string&& name) : 
    PadBase(typeid(T), PadBase::Direction::INPUT, std::forward<std::string>(name))
{
}

template <typename T>
inline InputPad<T>::~InputPad(){
    setSource(nullptr);
}



template <typename T>
inline void InputPad<T>::setSource(OutputPad<T>* src){
    if(m_source) m_source->m_consumers.erase(this);
    m_source = src;
    if(m_source) m_source->m_consumers.emplace(this);
}

template <typename T>
inline OutputPad<T>* InputPad<T>::getSource() const{
    return m_source;
}



template <typename T>
InputPad<T>::InputPad(const InputPad& other) :
    PadBase(other)
{
    setSource(other.m_source);
}

template <typename T>
InputPad<T>::InputPad(InputPad&& other) :
    PadBase(other)
{
    setSource(other.m_source);
    other.setSource(nullptr);
}

template <typename T>
InputPad<T>& InputPad<T>::operator=(const InputPad& other){
    static_cast<PadBase&>(this) = other;
    setSource(other.m_source);

    return *this;
}

template <typename T>
InputPad<T>& InputPad<T>::operator=(InputPad&& other){
    static_cast<PadBase&>(this) = std::move(other);
    setSource(other.m_source);
    other.setSource(nullptr);

    return *this;
}

template <typename T>
inline const std::shared_ptr<const T>& InputPad<T>::get() const{
    if(m_source) {
        return m_source->get();
    } else {
        return NO_SIGNAL;
    }
}

}