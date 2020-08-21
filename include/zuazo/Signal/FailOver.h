#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/Pimpl.h"

#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class FailOver 
	: public Layout
{
public:
	explicit FailOver(std::string name);
	FailOver(const FailOver& other) = delete;
	FailOver(FailOver&& other) = default;
	~FailOver() = default;

	FailOver&								operator=(const FailOver& other) = delete;
	FailOver&								operator=(FailOver&& other) = default;

	PadProxy<Input<T>>&						getInput();
	const PadProxy<Input<T>>&				getInput() const;
	PadProxy<Input<T>>&						getBackupInput();
	const PadProxy<Input<T>>&				getBackupInput() const;
	PadProxy<Output<T>>&					getOutput();
	const PadProxy<Output<T>>&				getOutput() const;

	static constexpr std::string_view		BACKUP_INPUT_NAME = "backup";

private:
	struct IO {
		Input<T> input = Input<T>(makeInputName<T>());
		Input<T> backup = Input<T>(BACKUP_INPUT_NAME);
		Output<T> output = Output<T>(makeOutputName<T>(), makePullCbk(output, input, backup));
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>						m_io;

	static PullCallback						makePullCbk(Output<T>& output, const Input<T>& input, const Input<T>& backup);

};
	
}

#include "FailOver.inl"