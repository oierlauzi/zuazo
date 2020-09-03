#include "Input.h"

namespace Zuazo::Signal {

/*
 * Input<T>
 */

template <typename T>
inline Input<T>::Input(std::string name) 
	: PadBase(std::move(name))
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
	return static_cast<Source*>(getSubject());
}



template <typename T>
inline void Input<T>::reset() {
	m_lastElement = Element();
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::pull() {
	m_lastElement = pullFromSource();
	return m_lastElement;
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::getLastElement() const {
	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const {
	const auto& newElement = pullFromSource();
	return m_lastElement != newElement;
}



template <typename T>
inline const typename Input<T>::Element& Input<T>::pullFromSource() const{
	auto* source = getSource();
	return source ? source->pull() : Source::NO_SIGNAL;
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