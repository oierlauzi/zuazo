#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/Pimpl.h"

#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class BinomialLayout 
	: public Layout
{
public:
	explicit BinomialLayout(std::string name, 
							std::string inputName = std::string(makeInputName<T>()), 
							std::string outputName = std::string(makeOutputName<T>()),
							PullCallback pullCbk = {} );
	BinomialLayout(const BinomialLayout& other) = delete;
	BinomialLayout(BinomialLayout&& other) = default;
	~BinomialLayout() = default;

	BinomialLayout&						operator=(const BinomialLayout& other) = delete;
	BinomialLayout&						operator=(BinomialLayout&& other) = default;

	PadProxy<Input<T>>&					getInput();
	const PadProxy<Input<T>>&			getInput() const;
	PadProxy<Output<T>>&				getOutput();
	const PadProxy<Output<T>>&			getOutput() const;

protected:
	Input<T>&							getInputPad();
	const Input<T>&						getInputPad() const;
	Output<T>&							getOutputPad();
	const Output<T>&					getOutputPad() const;

private:
	struct IO {
		IO(std::string inputName, std::string outputName, PullCallback pullcbk);
		~IO() = default;

		Input<T> input;
		Output<T> output;
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>					m_io;

};
	
}

#include "BinomialLayout.inl"