#include "BinomialLayout.h"

namespace Zuazo::Signal {

template<typename T>
inline BinomialLayout<T>::BinomialLayout(std::string name, std::string inputName, std::string outputName, PullCallback pullCbk)
	: Layout(std::move(name))
	, m_io({}, std::move(inputName), std::move(outputName), std::move(pullCbk))
{
	Layout::registerPads( { m_io->input, m_io->output } );
}



template<typename T>
inline Layout::PadProxy<Input<T>>& BinomialLayout<T>::getInput() {
	return Layout::makeProxy(getInputPad());
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& BinomialLayout<T>::getInput() const {
	return Layout::makeProxy(getInputPad());
}

template<typename T>
inline Layout::PadProxy<Output<T>>& BinomialLayout<T>::getOutput() {
	return Layout::makeProxy(getOutputPad());
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& BinomialLayout<T>::getOutput() const {
	return Layout::makeProxy(getOutputPad());
}



template<typename T>
inline Input<T>& BinomialLayout<T>::getInputPad() {
	return m_io->input;
}

template<typename T>
inline const Input<T>& BinomialLayout<T>::getInputPad() const {
	return m_io->input;
}

template<typename T>
inline Output<T>& BinomialLayout<T>::getOutputPad() {
	return m_io->output;
}

template<typename T>
inline const Output<T>& BinomialLayout<T>::getOutputPad() const {
	return m_io->output;
}



template<typename T>
inline BinomialLayout<T>::IO::IO(std::string inputName, std::string outputName, PullCallback pullcbk)
	: input(std::move(inputName))
	, output(std::move(outputName), std::move(pullcbk))
{
}

}