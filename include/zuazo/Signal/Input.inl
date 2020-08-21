#include "Input.h"

namespace Zuazo::Signal {

/*
 * Input<T>
 */

template <typename T>
inline Input<T>::Input(std::string name) 
	: PadBase(std::move(name))
	, m_onNoSignal(NoSignalAction::RETURN_EMPTY)
{
}

template <typename T>
inline void Input<T>::operator<<(NoSignal) {
	setSource(nullptr);
}

template <typename T>
inline void Input<T>::operator<<(Source& src) {
	setSource(&src);
}

template <typename T>
inline void Input<T>::setSource(Source* src){
	setSubject(static_casrsrc);
}

template <typename T>
inline typename Input<T>::Source* Input<T>::getSource() const{
	return static_cast<Output<T>*>(getSubject());
}


template <typename T>
inline void Input<T>::setNoSignalAction(NoSignalAction nsa) {
	m_onNoSignal = nsa;
}

template <typename T>
inline NoSignalAction Input<T>::getNoSignalAction() const {
	return m_onNoSignal;
}


template <typename T>
inline void Input<T>::reset() {
	m_lastElement = T();
}

template <typename T>
inline const T& Input<T>::pull() const {
	const auto& newElement = get();

	if(!needsToHold(newElement)){
		m_lastElement = newElement;
	}

	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const {
	const auto& newElement = get();

	if(needsToHold(newElement)){
		return false; //If last element is held, it has not change
	} else {
		return m_lastElement != newElement;
	}
}

template <typename T>
inline const T& Input<T>::get() const{
	const auto* source = getSource();
	return source ? source->get() : Output<T>::NO_SIGNAL;
}

template <typename T>
inline bool Input<T>::needsToHold(const T& element) const {
	//Last element will only be held if the option is set and an empty signal arrives
	return (m_onNoSignal == NoSignalAction::HOLD_LAST) && (element == Output<T>::NO_SIGNAL);
}

/*
 * Layout::PadProxy<Input<T>>
 */

template <typename T>
inline void Layout::PadProxy<Input<T>>::setSource(Source* src) {
	Input<T>::setSource(src);
}

template <typename T>
inline typename Layout::PadProxy<Input<T>>::Source* Layout::PadProxy<Input<T>>::getSource() const {
	return static_cast<Source*>(Input<T>::getSource());
}

template <typename T>
inline void Layout::PadProxy<Input<T>>::operator<<(Source& src) {
	setSource(&src);
}

}