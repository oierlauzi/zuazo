#include "Output.h"

#include <algorithm>

namespace Zuazo::Signal {

/*
 * Output<T>
 */

template <typename T>
inline const typename Output<T>::Element Output<T>::NO_SIGNAL = T();

template <typename T>
inline Output<T>::Output(	const Layout& layout, 
							std::string name, 
							PullCallback pullCbk ) noexcept
	: PadBase(layout, std::move(name))
	, m_pullCallback(std::move(pullCbk))
	, m_maxRecursion(1)
	, m_recursion(0)
	, m_lastElement()
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
		[] (Utils::Observer* obs) -> Consumer& {
			return static_cast<Consumer&>(*obs);
		}
	);

	return result;
}


template <typename T>
inline PadProxy<Output<T>>& Output<T>::getProxy() noexcept {
	return static_cast<PadProxy<Output>&>(*this);
}

template <typename T>
inline const PadProxy<Output<T>>& Output<T>::getProxy() const noexcept {
	return static_cast<PadProxy<const Output>&>(*this);
}


template <typename T>
inline void Output<T>::setPullCallback(PullCallback cbk) noexcept {
	m_pullCallback = std::move(cbk);
}

template <typename T>
inline const typename Output<T>::PullCallback& Output<T>::getPullCallback() const noexcept {
	return m_pullCallback;
}


template <typename T>
inline void Output<T>::setMaxRecursion(size_t rec) noexcept {
	m_maxRecursion = rec;
}

template <typename T>
inline size_t Output<T>::getMaxRecursion() const noexcept {
	return m_maxRecursion;
}

template <typename T>
inline size_t Output<T>::getRecursion() const noexcept {
	return m_recursion;
}




template <typename T>
inline void Output<T>::reset() noexcept {
	push(Element());
}

template <typename T>
void Output<T>::push(Element element) noexcept {
	m_lastElement = std::move(element);
}

template <typename T>
inline const typename Output<T>::Element& Output<T>::pull() noexcept {
	//Only invoke the callback if not exceeding the maximum recursion
	if(m_pullCallback && m_recursion < m_maxRecursion) {
		++m_recursion;
		m_pullCallback(*this);
		--m_recursion;
	}

	return m_lastElement;
}

template <typename T>
inline const typename Output<T>::Element& Output<T>::getLastElement() const noexcept {
	return m_lastElement;
}


/*
 * PadProxy<Output<T>>
 */

template <typename T>
inline typename PadProxy<Output<T>>::Consumers PadProxy<Output<T>>::getConsumers() const {
	Consumers result;

	const auto consumers = Output<T>::getConsumers();
	result.reserve(consumers.size());

	std::transform(
		consumers.cbegin(), consumers.cend(),
		std::back_inserter(result),
		[] (Input<T>& input) -> PadProxy<Input<T>>& {
			return static_cast<PadProxy<Input<T>>&>(input);
		}
	);

	return result;
}

/*
 * Get input
 */

template<typename T>
inline PadProxy<Output<T>>& getOutput(Layout& layout, std::string_view name) {
	auto* output = layout.getPad<Output<T>>(name);
	
	if(!output) {
		throw Exception("Requested output not found");
	}

	return *output;
}

template<typename T>
inline const PadProxy<Output<T>>& getOutput(const Layout& layout, std::string_view name) {
	const auto* output = layout.getPad<Output<T>>(name);
	
	if(!output) {
		throw Exception("Requested output not found");
	}

	return *output;
}

}