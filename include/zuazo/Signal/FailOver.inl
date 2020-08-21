#include "FailOver.h"

namespace Zuazo::Signal {

template<typename T>
inline FailOver<T>::FailOver(std::string name)
	: Layout(std::move(name))
{
	Layout::registerPads( {m_io->input, m_io->backup, m_io->output} );
}



template<typename T>
inline Layout::PadProxy<Input<T>>& FailOver<T>::getInput() {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& FailOver<T>::getInput() const {
	return Layout::makeProxy(m_io->input);
}

template<typename T>
inline Layout::PadProxy<Input<T>>& FailOver<T>::getBackupInput() {
	return Layout::makeProxy(m_io->backup);
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& FailOver<T>::getBackupInput() const {
	return Layout::makeProxy(m_io->backup);
}

template<typename T>
inline Layout::PadProxy<Output<T>>& FailOver<T>::getOutput() {
	return Layout::makeProxy(m_io->output);
}

template<typename T>
inline const Layout::PadProxy<Output<T>>& FailOver<T>::getOutput() const {
	return Layout::makeProxy(m_io->output);
}



template<typename T>
inline PullCallback FailOver<T>::makePullCbk(Output<T>& output, const Input<T>& input, const Input<T>& backup) {
	return [&output, &input, &backup] () -> void {
		//Retrieve an element from the primary input
		const auto& x = input.pull();

		if(x != Input<T>::NO_SIGNAL) {
			//Primary input OK
			output.push(x);
		} else {
			//Primary input failed, switch to backup input
			output.push(backup.pull());
		}
	};
}

}