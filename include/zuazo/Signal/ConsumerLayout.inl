#include "ConsumerLayout.h"

namespace Zuazo::Signal {

template<typename T>
inline ConsumerLayout<T>::ConsumerLayout(std::string name, std::string inputName)
	: Layout(std::move(name))
	, m_io(std::move(inputName))
{
	Layout::registerPad(m_io->input);
}



template<typename T>
inline Layout::PadProxy<Input<T>>& ConsumerLayout<T>::getInput() {
	return Layout::makeProxy(getInputPad());
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& ConsumerLayout<T>::getInput() const {
	return Layout::makeProxy(getInputPad());
}



template<typename T>
inline Input<T>& ConsumerLayout<T>::getInputPad() {
	return m_io->input;
}

template<typename T>
inline const Input<T>& ConsumerLayout<T>::getInputPad() const {
	return m_io->input;
}



template<typename T>
inline ConsumerLayout<T>::IO::IO(std::string inputName)
	: input(std::move(inputName))
{
}

}