#include "Output.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline void Output<T>::reset(){
	m_lastElement.reset();
}

template <typename T>
inline void Output<T>::push(T&& element){
	m_lastElement = std::move(element);
}

template <typename T>
inline const T& Output<T>::get() const{
	return m_lastElement;
}

}
