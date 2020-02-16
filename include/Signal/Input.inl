#include "Input.h"

namespace Zuazo::Signal {

template <typename T>
inline Input<T>::Input(std::string&& name, Layout* owner)
	: InputPad<T>(std::move(name), owner)
{
}

template <typename T>
inline void Input<T>::setHold(bool hold){
	m_hold = hold;
}

template <typename T>
inline bool Input<T>::getHold() const{
	return m_hold;
}

template <typename T>
inline void Input<T>::reset() {
	m_lastElement.reset();
}

template <typename T>
inline const T& Input<T>::get() const{
	const auto& newElement = InputPad<T>::get();

	if(newElement != InputPad<T>::NO_SIGNAL || !m_hold){
		m_lastElement = newElement;
	}

	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const{
	const auto& newElement = InputPad<T>::get();

	if(newElement != InputPad<T>::NO_SIGNAL || !m_hold){
		return m_lastElement == newElement;
	} else {
		return true; 
	}
}

}