#include "Queue.h"

namespace Zuazo::Signal {

template<typename T>
inline Queue<T>::Queue(std::string name)
	: Layout(std::move(name), { m_input, m_output })
	, ProcessorLayout<T, T>(makeProxy(m_input), makeProxy(m_output))
{
}


template<typename T>
inline void Queue<T>::updateInput() {
	m_queue.push(m_input.pull());
}

template<typename T>
inline void Queue<T>::updateOutput() {
	if(m_queue.emplty()) {
		//Could not retrieve an element
		m_output.reset();
	} else {
		//There is at least one element
		m_output.getOutput().push(std::move(m_queue.front()));
		m_queue.pop();
	}
}

}