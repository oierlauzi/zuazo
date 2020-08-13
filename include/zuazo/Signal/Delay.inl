#include "Delay.h"

namespace Zuazo::Signal {

template<typename T>
inline Delay<T>::Delay(std::string name, Duration delay)
	: Layout(std::move(name))
	, m_io()
	, m_delay(delay)
{
	Layout::registerPads( {m_io->input, m_io->output} );
}

template<typename T>
inline Layout::PadProxy<Input<T>>& Delay<T>::getInput() {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& Delay<T>::getInput() const {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline Layout::PadProxy<Output<T>>& Delay<T>::getOutput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& Delay<T>::getOutput() const {
	return Layout::makeProxy(m_io->output);
}



template<typename T>
inline void Delay<T>::setDelay(Duration delay) {
	m_delay = delay;
}

template<typename T>
inline Duration Delay<T>::getDelay() const {
	return m_delay;
}



template<typename T>
inline void Delay<T>::update(TimePoint now) {
	//Push a new element from the input into the delay line
	m_delayLine.push_back(
		Packet{
			now, 
			m_io->input.pull()
		}
	);

	//Push all elements older than delay into the output
	while(m_delayLine.front().time + m_delay >= now) {
		m_io->output.push(m_delayLine.front().data);
		m_delayLine.pop_front();
	}
}

}