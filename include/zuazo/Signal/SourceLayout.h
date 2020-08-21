#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/Pimpl.h"

#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class SourceLayout 
	: public Layout
{
public:
	SourceLayout(	std::string name, 
					std::string outputName = makeOutputName<T>(),
					PullCallback pullCbk = {} );
	SourceLayout(const SourceLayout& other) = delete;
	SourceLayout(SourceLayout&& other) = default;
	~SourceLayout() = default;

	SourceLayout&						operator=(const SourceLayout& other) = delete;
	SourceLayout&						operator=(SourceLayout&& other) = default;

	PadProxy<Output<T>>&				getOutput();
	const PadProxy<Output<T>>&			getOutput() const;

protected:
	Output<T>&							getOutputPad();
	const Output<T>&					getOutputPad() const;

private:
	struct IO {
		IO(std::string outputName, PullCallback pullcbk);
		~IO() = default;

		Output<T> output;
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>					m_io;

};

	
}

#include "SourceLayout.inl"