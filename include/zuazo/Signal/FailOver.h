#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "../Utils/Pimpl.h"

namespace Zuazo::Signal {

template <typename T>
class FailOver 
	: public Layout
{
public:
	FailOver(std::string name);
	FailOver(const DummyPad& other) = delete;
	FailOver(DummyPad&& other) = default;
	~FailOver() = default;

	FailOver&								operator=(const FailOver& other) = delete;
	FailOver&								operator=(FailOver&& other) = default;

	PadProxy<Input<T>>&						getInput();
	const PadProxy<Input<T>>&				getInput() const;
	PadProxy<Input<T>>&						getBackupInput();
	const PadProxy<Input<T>>&				getBackupInput() const;
	PadProxy<Output<T>>&					getOutput();
	const PadProxy<Output<T>>&				getOutput() const;

private:
	struct IO {
		Input<T> input = Input<T>("in");
		Input<T> backup = Input<T>("backup");
		Output<T> output = Output<T>("out", makePullCbk(output, input, backup));
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>						m_io;

	static typename Output<T>::PullCallback	makePullCbk(Output<T>& output, const Input<T>& input, const Input<T>& backup);

};
	
}

#include "FailOver.inl"