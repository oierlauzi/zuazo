#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "../Utils/Pimpl.h"

namespace Zuazo::Signal {

template <typename T>
class DummyPad {
public:
	explicit DummyPad(std::string name);
	DummyPad(const DummyPad& other) = delete;
	DummyPad(DummyPad&& other) = default;
	~DummyPad() = default;

	DummyPad&							operator=(const DummyPad& other) = delete;
	DummyPad&							operator=(DummyPad&& other) = default;

	Layout::PadProxy<Input<T>>&			getInput();
	const Layout::PadProxy<Input<T>>&	getInput() const;
	Layout::PadProxy<Output<T>>&		getOutput();
	const Layout::PadProxy<Output<T>>&	getOutput() const;

	//Magic operator in order to concatenate
	Layout::PadProxy<Output<T>>&		operator<<(Layout::PadProxy<Output<T>>& src);

private:
	struct IO {
		IO(std::string name);
		~IO() = default;

		Input<T> input;
		Output<T> output;

	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>					m_io;

};

	
}

#include "DummyPad.inl"