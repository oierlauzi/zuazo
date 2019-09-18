#include "Source.h"

#include <utility>

namespace Zuazo::Stream {

template <typename T>
inline void Source<T>::reset(){
    m_lastElement.reset();
}

template <typename T>
inline void Source<T>::push(std::shared_ptr<const T>&& element){
    m_lastElement = std::forward<std::shared_ptr<const T>>(element);
}

template <typename T>
inline const std::shared_ptr<const T>& Source<T>::get() const{
    return m_lastElement;
}

}
