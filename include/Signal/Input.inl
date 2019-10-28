#include "Input.h"

namespace Zuazo::Signal {

template <typename T>
inline typename Input<T>::BackupSignal Input<T>::backupSignal;

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
inline const std::shared_ptr<const T>& Input<T>::get() const{
	auto newElement = InputPad<T>::get();

	if(newElement){
		m_lastElement = newElement;
	}else if(!m_hold){
		m_lastElement = backupSignal.get();
	}

	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const{
	auto newElement = InputPad<T>::get();

	if(newElement){
		return m_lastElement == newElement;
	}else if(!m_hold){
		return m_lastElement == backupSignal.get();
	}else {
		return true; 
	}
}

}