#include "Pimpl.h"

namespace Zuazo::Utils {

template<typename T>
template<typename ...Args> 
Pimpl<T>::Pimpl(Args&&... args)
	: m_ptr(std::make_unique<T>(std::forward<Args>(args)...))
{
}

template<typename T>
T* Pimpl<T>::operator->() {
	return m_ptr.get();
}

template<typename T>
const T* Pimpl<T>::operator->() const {
	return m_ptr.get();
}

template<typename T>
T& Pimpl<T>::operator*() {
	return *m_ptr;
}

template<typename T>
const T& Pimpl<T>::operator*() const {
	return *m_ptr;
}

template<typename T>
Pimpl<T>::operator bool() const {
	return m_ptr;
}


}
