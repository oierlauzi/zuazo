#include "DummyPad.h"

namespace Zuazo::Signal {

template<typename T>
inline DummyPad<T>::DummyPad(std::string name)
	: ProcessorLayout<T, T>(Layout::makeProxy(m_input), Layout::makeProxy(m_output))
	, m_input(name)
	, m_output(std::move(name), makePullCallback(m_input))
{
}

template<typename T>
inline typename Output<T>::PullCallback	DummyPad<T>::makePullCallback(Input<T>& input) {
	return [&input] (Output<T>& output) {
		 output.push(input.pull()); 
	};
}

}