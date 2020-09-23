#include "Synchronizer.h"

#include <cassert>

namespace Zuazo::Signal {

template<typename T>
inline Synchronizer<T>::Synchronizer(std::string name, size_t maxDelay, size_t maxDropped)
	: Layout(std::move(name), { m_input, m_output })
	, ProcessorLayout<T, T>(makeProxy(m_input), makeProxy(m_output))
	, m_queue(maxDelay)
	, m_maxDropped(maxDropped)
	, m_dropped(0)
{
}



template<typename T>
inline size_t Synchronizer<T>::getMaxDelay() const {
	return m_queue.capacity();
}

template<typename T>
inline size_t Synchronizer<T>::getDelay() const {
	return m_queue.size();
}



template<typename T>
inline size_t Synchronizer<T>::getMaxDropped() const {
	return m_maxDropped;
}

template<typename T>
inline size_t Synchronizer<T>::getDropped() const {
	return m_dropped;
}



template<typename T>
inline void Synchronizer<T>::updateInput() {
	if(!m_queue.full()) {
		m_input.pull());
	}
}

template<typename T>
inline void Synchronizer<T>::updateOutput() {
	if(m_queue.size() > 0) {
		//There is at least one element
		m_dropped.load(0);
		m_output.push(std::move(m_queue.front()));
		m_queue.pop();
	} else {
		//Could not retrieve an element
		if(m_dropped.fetch_add(1) >= getMaxDropped()) {
			m_output.reset();
		}
	}
}

}