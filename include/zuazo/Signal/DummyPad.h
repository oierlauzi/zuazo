#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "../Utils/Pimpl.h"

#include <deque>

namespace Zuazo::Signal {

template <typename T>
class DummyPad 
	: public Layout
{
public:
	DummyPad(std::string name);
	DummyPad(const DummyPad& other) = delete;
	DummyPad(DummyPad&& other) = default;
	~DummyPad() = default;

	DummyPad&						operator=(const DummyPad& other) = delete;
	DummyPad&						operator=(DummyPad&& other) = default;

	PadProxy<Input<T>>&				getInput();
	const PadProxy<Input<T>>&		getInput() const;
	PadProxy<Output<T>>&			getOutput();
	const PadProxy<Output<T>>&		getOutput() const;

private:
	struct IO {
		IO(std::string name);
		~IO() = default;


		Input<T> input;
		Output<T> output;
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>				m_io;

	static typename Output<T>::PullCallback	makePullCbk(Output<T>& output, const Input<T>& input);

};

	
}

#include "DummyPad.inl"