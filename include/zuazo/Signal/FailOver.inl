#include "FailOver.h"

namespace Zuazo::Signal {

template<typename T>
inline FailOver<T>::FailOver(std::string name)
	: Layout(std::move(name), { m_backupInput, m_input, m_output })
	, ProcessorLayout<T, T>(makeProxy(m_input), makeProxy(m_output))
	, m_backupInput(std::string(BACKUP_INPUT_NAME))
	, m_input()
	, m_output(std::string(makeOutputName<T>()), makePullCallback(m_backupInput, m_input))
{
}



template<typename T>
inline Layout::PadProxy<Input<T>>& FailOver<T>::getBackupInput() noexcept {
	return m_backupInput;
}

template<typename T>
inline const Layout::PadProxy<Input<T>>& FailOver<T>::getBackupInput() const noexcept {
	return m_backupInput;
}

template<typename T>
inline typename Output<T>::PullCallback FailOver<T>::makePullCallback(Input<T>& backupInput, Input<T>& input) {
	return [&backup, &input] (Output<T>& output) {
		//Retrieve an element from the primary input
		const auto& x = input.pull();

		if(x != Output<T>::NO_SIGNAL) {
			//Primary input OK
			output.push(x);
		} else {
			//Primary input failed, switch to backup input
			output.push(backup.pull());
		}
	};
}

}