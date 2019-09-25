#include "Output.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline void Output<T>::reset(){
    m_lastElement.reset();
}

template <typename T>
inline void Output<T>::push(std::shared_ptr<const T>&& element){
    m_lastElement = std::forward<std::shared_ptr<const T>>(element);
}

template <typename T>
inline const std::shared_ptr<const T>& Output<T>::get() const{
    return m_lastElement;
}

}
