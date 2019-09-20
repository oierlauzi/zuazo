#include "Consumer.h"

namespace Zuazo::Signal {

template <typename T>
inline typename Consumer<T>::BackupSignal Consumer<T>::backupSignal;

template <typename T>
inline void Consumer<T>::setHold(bool hold){
    m_hold = hold;
}

template <typename T>
inline bool Consumer<T>::getHold() const{
    return m_hold;
}

template <typename T>
inline void Consumer<T>::reset() {
    m_lastElement.reset();
}

template <typename T>
inline const std::shared_ptr<const T>& Consumer<T>::get() const{
    auto newElement = ConsumerPad<T>::get();

    if(newElement){
        m_lastElement = newElement;
    }else if(!m_hold){
        m_lastElement = backupSignal.get();
    }

    return m_lastElement;
}

template <typename T>
inline bool Consumer<T>::hasChanged() const{
    auto newElement = ConsumerPad<T>::get();

    if(newElement){
        return m_lastElement == newElement;
    }else if(!m_hold){
        return m_lastElement == backupSignal.get();
    }else {
        return true; 
    }
}

}