#include "SourceLayout.h"

namespace Zuazo::Signal {

template<typename T>
inline SourceLayout<T>::SourceLayout(std::string name, std::string outputName, PullCallback pullCbk)
	: Layout(std::move(name))
	, m_io({}, std::move(outputName), std::move(pullCbk))
{
	Layout::registerPad(m_io->output);
}



template<typename T>
inline Layout::PadProxy<Output<T>>& SourceLayout<T>::getOutput() {
	return Layout::makeProxy(getOutputPad());
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& SourceLayout<T>::getOutput() const {
	return Layout::makeProxy(getOutputPad());
}



template<typename T>
inline Output<T>& SourceLayout<T>::getOutputPad() {
	return m_io->output;
}

template<typename T>
inline const Output<T>& SourceLayout<T>::getOutputPad() const {
	return m_io->output;
}



template<typename T>
inline SourceLayout<T>::IO::IO(std::string outputName, PullCallback pullcbk)
	: output(std::move(outputName), std::move(pullcbk))
{
}

}