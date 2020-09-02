#include "DummyPad.h"

namespace Zuazo::Signal {

template<typename T>
inline DummyPad<T>::DummyPad(std::string name)
	: m_io(std::move(name))
{
}

template<typename T>
inline Layout::PadProxy<Input<T>>& DummyPad<T>::getInput() {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& DummyPad<T>::getInput() const {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline Layout::PadProxy<Output<T>>& DummyPad<T>::getOutput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& DummyPad<T>::getOutput() const {
	return Layout::makeProxy(m_io->output);
}



template<typename T>
inline Layout::PadProxy<Output<T>>& DummyPad<T>::operator<<(Layout::PadProxy<Output<T>>& src) {
	getInput() << src;
	return getOutput();
}



template<typename T>
inline DummyPad<T>::IO::IO(std::string name)
	: input(name, 				std::bind(&Output<T>::push, std::ref(output), std::placeholders::_1))
	, output(std::move(name), 	std::bind(&Input<T>::pull, std::ref(input)))
{
}

}