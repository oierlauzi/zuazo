#include "DummyPad.h"

#include <cassert>

namespace Zuazo::Signal {

template<typename T>
inline DummyPad<T>::DummyPad(std::string name)
	: ProcessorLayout<T, T>(Layout::makeProxy(m_input), Layout::makeProxy(m_output))
	, m_input(name)
	, m_output(std::move(name), makePullCallback(m_input))
{
}


template<typename T>
inline void DummyPad<T>::setName(std::string name) {
	m_input.setName(name);
	m_output.setName(std::move(name));
}

template<typename T>
inline const std::string& DummyPad<T>::getName() const noexcept {
	const auto& result = m_input.getName();
	assert(m_output.getName() == result);
	return result;
}



template<typename T>
inline typename Output<T>::PullCallback	DummyPad<T>::makePullCallback(Input<T>& input) {
	return [&input] (Output<T>& output) {
		 output.push(input.pull()); 
	};
}

}