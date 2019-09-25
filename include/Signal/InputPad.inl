#include "InputPad.h"

#include "OutputPad.h"

namespace Zuazo::Signal {

template <typename T>
inline const std::shared_ptr<const T> InputPad<T>::NO_SIGNAL;

template <typename T>
inline InputPad<T>::InputPad(std::string&& name) : 
    PadBase(std::forward<std::string>(name), PadBase::INPUT, typeid(T))
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
inline const std::shared_ptr<const T>& InputPad<T>::get() const{
    if(m_source) {
        return m_source->get();
    } else {
        return NO_SIGNAL;
    }
}

}