#include "DummyPad.h"

#include <cassert>

namespace Zuazo::Signal {

template<typename T>
inline DummyPad<T>::DummyPad(const Layout& layout, std::string name)
	: ProcessorLayout<T, T>(m_input.getProxy(), m_output.getProxy())
	, m_input(layout, name)
	, m_output(layout, std::move(name), makePullCallback(m_input))
{
}



template<typename T>
inline void DummyPad<T>::setLayout(const Layout& layout) noexcept {
	m_input.setLayout(layout);
	m_output.setLayout(layout);	
}

template<typename T>
inline const Layout& DummyPad<T>::getLayout() const noexcept {
	return m_input.getLayout();
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