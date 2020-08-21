#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/Pimpl.h"

#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class ConsumerLayout 
	: public Layout
{
public:
	explicit ConsumerLayout(std::string name, 
							std::string inputName = makeInputName<T>() );
	ConsumerLayout(const ConsumerLayout& other) = delete;
	ConsumerLayout(ConsumerLayout&& other) = default;
	~ConsumerLayout() = default;

	ConsumerLayout&						operator=(const ConsumerLayout& other) = delete;
	ConsumerLayout&						operator=(ConsumerLayout&& other) = default;

	PadProxy<Input<T>>&					getInput();
	const PadProxy<Input<T>>&			getInput() const;

protected:
	Input<T>&							getInputPad();
	const Input<T>&						getInputPad() const;

private:
	struct IO {
		IO(std::string inputName);
		~IO() = default;

		Input<T> input;
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>					m_io;

};

	
}

#include "ConsumerLayout.inl"