#include "Input.h"

#include "../Exception.h"

namespace Zuazo::Signal {

/*
 * Input<T>
 */

template <typename T>
inline Input<T>::Input(const Layout& layout, std::string name) noexcept
	: PadBase(layout, std::move(name))
{
}



template <typename T>
inline void Input<T>::operator<<(NoSignal) noexcept {
	setSource(nullptr);
}

template <typename T>
inline void Input<T>::operator<<(Source& src) noexcept {
	setSource(&src);
}

template <typename T>
inline void Input<T>::setSource(Source* src) noexcept {
	setSubject(static_cast<Zuazo::Utils::Subject*>(src));
}

template <typename T>
inline typename Input<T>::Source* Input<T>::getSource() const noexcept {
	return static_cast<Source*>(getSubject());
}


template <typename T>
inline PadProxy<Input<T>>& Input<T>::getProxy() noexcept {
	return static_cast<PadProxy<Input>&>(*this);
}

template <typename T>
inline const PadProxy<Input<T>>& Input<T>::getProxy() const noexcept {
	return static_cast<const PadProxy<Input>&>(*this);
}


template <typename T>
inline void Input<T>::reset() noexcept {
	m_lastElement = Element();
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::pull() noexcept {
	m_lastElement = pullFromSource();
	return m_lastElement;
}

template <typename T>
inline const typename Input<T>::Element& Input<T>::getLastElement() const noexcept {
	return m_lastElement;
}

template <typename T>
inline bool Input<T>::hasChanged() const noexcept {
	const auto& newElement = pullFromSource();
	return m_lastElement != newElement;
}



template <typename T>
inline const typename Input<T>::Element& Input<T>::pullFromSource() const noexcept {
	auto* source = getSource();
	return source ? source->pull() : Source::NO_SIGNAL;
}


/*
 * PadProxy<Input<T>>
 */

template <typename T>
inline void PadProxy<Input<T>>::setSource(Source* src) noexcept {
	Input<T>::setSource(src);
}

template <typename T>
inline typename PadProxy<Input<T>>::Source* PadProxy<Input<T>>::getSource() const noexcept {
	return static_cast<Source*>(Input<T>::getSource());
}

template <typename T>
inline void PadProxy<Input<T>>::operator<<(Source& src) noexcept {
	setSource(&src);
}

/*
 * Get input
 */

template<typename T>
inline PadProxy<Input<T>>& getInput(Layout& layout, std::string_view name) {
	auto* input = layout.getPad<Input<T>>(name);
	
	if(!input) {
		throw Exception("Requested input not found");
	}

	return *input;
}

template<typename T>
inline const PadProxy<Input<T>>& getInput(const Layout& layout, std::string_view name) {
	const auto* input = layout.getPad<Input<T>>(name);
	
	if(!input) {
		throw Exception("Requested input not found");
	}

	return *input;
}

}