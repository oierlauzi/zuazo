#include "Output.h"

#include <algorithm>

namespace Zuazo::Signal {

/*
 * Output<T>
 */

template <typename T>
inline Output<T>::Output(std::string name, PullCallback pullCbk)
	: PadBase(std::move(name))
	, m_pullCallback(std::move(pullCbk))
{
}

template <typename T>
inline typename Output<T>::Consumers Output<T>::getConsumers() const {
	const auto& observers = getObservers();

	Consumers result;
	result.reserve(observers.size());

	std::transform(
		observers.cbegin(), observers.cend(),
		std::back_inserter(result),
		[] (Utils::Observer* obs) -> Input<T>& {
			return static_cast<Input<T>&>(*obs);
		}
	);

	return result;
}

template <typename T>
inline void Output<T>::reset() {
	m_lastElement = T();
}

template <typename T>
void Output<T>::push(T element) {
	m_lastElement = std::move(element);
}

template <typename T>
inline const T& Output<T>::pull() const {
	if(m_pullCallback) m_pullCallback();
	return m_lastElement;
}

template <typename T>
inline void Output<T>::setPullCallback(PullCallback cbk) {
	m_pullCallback = std::move(cbk);
}

template <typename T>
inline const typename Output<T>::PullCallback& Output<T>::getPullCallback() const {
	return m_pullCallback;
}


/*
 * Layout::PadProxy<Output<T>>
 */

template <typename T>
inline Layout::PadProxy<Output<T>>::Consumers Layout::PadProxy<Output<T>>::getConsumers() const {
	Consumers result;

	const auto consumers = Output<T>::getConsumers();
	result.reserve(consumers.size());

	std::transform(
		consumers.cbegin(), consumers.cend(),
		std::back_inserter(result),
		[] (Input<T>& input) -> Layout::PadProxy<Input<T>>& {
			return static_cast<Layout::PadProxy<Input<T>>&>(input);
		}
	);

	return result;
}

}