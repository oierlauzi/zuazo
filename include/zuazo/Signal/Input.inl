#include "Input.h"

namespace Zuazo::Signal {

/*
 * Input<T>
 */

template <typename T>
inline Input<T>::Input(std::string name, PushCallback pushCbk) 
	: PadBase(std::move(name))
	, m_pushCallback(std::move(pushCbk))
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
	setSubject(static_cast<Zuazo::Utils::Subject*>(src));
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
inline void Input<T>::setPushCallback(PushCallback cbk) {
	m_pushCallback = std::move(cbk);
}

template <typename T>
inline const typename Input<T>::PushCallback& Input<T>::getPushCallback() const {
	return m_pushCallback;
}



template <typename T>
inline void Input<T>::reset() {
	m_lastElement = Element();
}

template <typename T>
void Input<T>::push(const Element& el) {
	if(m_pushCallback){
		m_lastElement = el;
		m_pushCallback(m_lastElement);
	} 
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::pull() {
	const auto& newElement = pullFromSource();

	if(!needsToHold(newElement)){
		m_lastElement = newElement;
	}

	return m_lastElement;
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::getLastElement() const {
	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const {
	const auto& newElement = pullFromSource();

	if(needsToHold(newElement)){
		return false; //If last element is held, it has not change
	} else {
		return m_lastElement != newElement;
	}
}



template <typename T>
inline const typename Input<T>::Element& Input<T>::pullFromSource() const{
	auto* source = getSource();
	return source ? source->pull() : Source::NO_SIGNAL;
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

/*
 * Get input
 */

template<typename T>
inline Layout::PadProxy<Input<T>>& getInput(Layout& layout, std::string_view name) {
	return layout.getPad<Input<T>>(name);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& getInput(const Layout& layout, std::string_view name) {
	return layout.getPad<Input<T>>(name);
}

}