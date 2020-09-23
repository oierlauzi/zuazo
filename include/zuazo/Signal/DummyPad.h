#pragma once 

#include "ProcessorLayout.h"
#include "Input.h"
#include "Output.h"

namespace Zuazo::Signal {

template <typename T>
class DummyPad 
	: public ProcessorLayout<T, T>
{
public:
	explicit DummyPad(std::string name);
	DummyPad(const DummyPad& other) = delete;
	~DummyPad() = default;

	DummyPad&								operator=(const DummyPad& other) = delete;

private:
	Input<T>								m_input;
	Output<T>								m_output;

	static typename Output<T>::PullCallback	makePullCallback(Input<T>& input);

};

	
}

#include "DummyPad.inl"