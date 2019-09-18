#include "Consumer.h"

namespace Zuazo::Stream {

template <typename T>
typename Consumer<T>::BackupSignal Consumer<T>::onNoSignal;

template <typename T>
void Consumer<T>::reset() {
    m_lastElement.reset();
}

template <typename T>
const std::shared_ptr<const T>& Consumer<T>::get() const{
    m_lastElement = reqElement();
    return m_lastElement;
}

template <typename T>
bool Consumer<T>::hasChanged() const{
    return m_lastElement == reqElement();
}

template <typename T>
const std::shared_ptr<const T>& Consumer<T>::reqElement() const{
    const std::shared_ptr<const T>& newElement = ConsumerPad<T>::get();

    if(newElement) {
        return newElement;
    } else {
        return onNoSignal.get();
    }
}
}