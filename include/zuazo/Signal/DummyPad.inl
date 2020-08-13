#include "DummyPad.h"

namespace Zuazo::Signal {

template<typename T>
DummyPad<T>::DummyPad(std::string name)
	: Layout(name)
	, m_io(std::move(name))
{
	Layout::registerPads( {m_io->input, m_io->output} );
}

template<typename T>
Layout::PadProxy<Input<T>>& DummyPad<T>::getInput() {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
const Layout::PadProxy<Input<T>>& DummyPad<T>::getInput() const {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
Layout::PadProxy<Output<T>>& DummyPad<T>::getOutput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
const Layout::PadProxy<Output<T>>& DummyPad<T>::getOutput() const {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
DummyPad<T>::IO::IO(std::string name)
	: input(name)
	, output(
		std::move(name), 
		[&input, &output] () {
			output.push(input.pull());
		}
	)
{
}

}