#include "AsyncInput.h"

namespace Zuazo::Signal {

template<typename T>
inline AsyncInput<T>::AsyncInput(std::string name)
	: Layout(name)
	, m_input(std::move(name))
	, m_maxDelay(m_queue.capacity())
{
	Layout::registerPad(*m_input);
}



template<typename T>
inline Layout::PadProxy<Input<T>>& AsyncInput<T>::getInput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& AsyncInput<T>::getInput() const {
	return Layout::makeProxy(m_io->output);
}



template<typename T>
inline size_t AsyncInput<T>::getCapacity() const {
	return m_queue.capacity();
}

template<typename T>
inline void AsyncInput<T>::setMaxDelay(size_t maxDelay) {
	assert(maxDelay <= getCapacity());
	m_maxDelay = maxDelay;
}

template<typename T>
inline size_t AsyncInput<T>::getMaxDelay() const {
	return m_maxDelay;
}

template<typename T>
inline size_t AsyncInput<T>::getDelay() const {
	return m_queue.size();
}


template<typename T>
inline T AsyncInput<T>::pull() {
	if(m_queue.size() > 0) {
		//There is at least one element
		T result = m_queue.front();
		m_queue.pop();
		return result;
	} else {
		return T();
	}
}

template<typename T>
inline void AsyncInput<T>::update() {
	if(m_queue.size() < m_maxDelay) {
		m_queue.push(m_input->pull());
	}
}

}