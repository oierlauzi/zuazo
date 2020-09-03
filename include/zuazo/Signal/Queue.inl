#include "Queue.h"

namespace Zuazo::Signal {

template<typename T>
inline Queue<T>::Queue(std::string name)
	: BinomialLayout<T>(std::move(name))
{
}


template<typename T>
inline void Queue<T>::updateInput() {
	m_queue.push(getInputPad().pull());
}

template<typename T>
inline void Queue<T>::updateOutput() {
	if(m_queue.emplty()) {
		//Could not retrieve an element
		getOutputPad().reset();
	} else {
		//There is at least one element
		getOutputPad().push(std::move(m_queue.front()));
		m_queue.pop();
	}
}

}