#include "AsyncOutput.h"

namespace Zuazo::Signal {

template<typename T>
inline AsyncOutput<T>::AsyncOutput(std::string name)
	: Layout(name)
	, m_output(std::move(name))
	, m_maxDelay(m_queue.capacity())
	, m_maxDropped(0)
	, m_dropped(0)
{
	Layout::registerPad(*m_output);
}



template<typename T>
inline Layout::PadProxy<Output<T>>& AsyncOutput<T>::getOutput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& AsyncOutput<T>::getOutput() const {
	return Layout::makeProxy(m_io->output);
}



template<typename T>
inline size_t AsyncOutput<T>::getCapacity() const {
	return m_queue.capacity();
}

template<typename T>
inline void AsyncOutput<T>::setMaxDelay(size_t maxDelay) {
	assert(maxDelay <= getCapacity());
	m_maxDelay = maxDelay;
}

template<typename T>
inline size_t AsyncOutput<T>::getMaxDelay() const {
	return m_maxDelay;
}

template<typename T>
inline size_t AsyncOutput<T>::getDelay() const {
	return m_queue.size();
}



template<typename T>
inline void AsyncOutput<T>::setMaxDropped(size_t maxDropped) {
	return m_maxDropped = maxDropped;
}

template<typename T>
inline size_t AsyncOutput<T>::getMaxDropped() const {
	return m_maxDropped;
}

template<typename T>
inline size_t AsyncOutput<T>::getDropped() const {
	return m_dropped;
}



template<typename T>
inline void AsyncOutput<T>::push(T element) {
	if(m_queue.size() < m_maxDelay) {
		m_queue.push(std::move(element));
	}
}

template<typename T>
inline void AsyncOutput<T>::update() {
	if(m_queue.size() > 0) {
		//There is at least one element
		m_dropped = 0;
		m_io->output.push(std::move(m_queue.front()));
		m_queue.pop();
	} else {
		//Could not retrieve an element
		if(++m_dropped > m_maxDropped) {
			m_io->output.reset();
		}
	}
}

}