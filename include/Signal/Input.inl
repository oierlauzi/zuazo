#include "Input.h"

namespace Zuazo::Signal {

/*
 * Input
 */

template <typename T>
inline const T Input<T>::NO_SIGNAL = T();

template <typename T>
template <typename Str>
inline Input<T>::Input(Str&& name) 
	: PadBase(std::forward<Str>(name))
{
}


template <typename T>
inline void Input<T>::setSource(Output<T>* src){
	m_source = src;
}

template <typename T>
inline Output<T>* Input<T>::getSource() const{
	return m_source;
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
	return m_source ? m_source->get() : NO_SIGNAL;
}

template <typename T>
inline bool Input<T>::needsToHold(const T& element) const {
	//Last element will only be held if the option is set and an empty signal arrives
	return (m_onNoSignal == NoSignalAction::HOLD_LAST) && (element == NO_SIGNAL);
}

/*
 * Layout::PadProxy<Input<T>>
 */

template <typename T>
inline Layout::PadProxy<Input<T>>::PadProxy(Input<T>& pad)
	: PadProxy<PadBase>(pad)
{
}

template <typename T>
inline void Layout::PadProxy<Input<T>>::setSource(Layout::PadProxy<Output<T>>* src) {
	return get<Input<T>>().setSource(src->template get<Output<T>>());
}

template <typename T>
inline Layout::PadProxy<Output<T>>* Layout::PadProxy<Input<T>>::getSource() const {
	return get<Input<T>>().getSource();
}

template <typename T>
inline void Layout::PadProxy<Input<T>>::operator<<(Layout::PadProxy<Output<T>>& src) {
	setSource(&src);
}

template <typename T>
inline void Layout::PadProxy<Input<T>>::operator<<(NoSignal) {
	setSource(nullptr);
}


template <typename T>
inline void Layout::PadProxy<Input<T>>::setNoSignalAction(NoSignalAction nsa) {
	return get<Input<T>>().setNoSignalAction(nsa);
}

template <typename T>
inline NoSignalAction Layout::PadProxy<Input<T>>::getNoSignalAction() const {
	return get<Input<T>>().getNoSignalAction();
}

/*
 * getInput(s) methods
 */

template <typename T>
inline std::vector<Layout::PadProxy<Input<T>>> getInputs(Layout& layout) {
	return layout.getPads<Input<T>>();
}

template <typename T>
inline std::vector<const Layout::PadProxy<Input<T>>> getInputs(const Layout& layout){
	return layout.getPads<Input<T>>();
}

template <typename T>
inline Layout::PadProxy<Input<T>> getInput(Layout& layout, std::string_view str) {
	return layout.getPad<Input<T>>(str);
}

template <typename T>
inline const Layout::PadProxy<Input<T>> getInput(const Layout& layout, std::string_view str) {
	return layout.getPad<Input<T>>(str);
}

}