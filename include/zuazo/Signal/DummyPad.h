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
	DummyPad(DummyPad&& other) = default;
	~DummyPad() = default;

	DummyPad&								operator=(const DummyPad& other) = delete;
	DummyPad&								operator=(DummyPad&& other) = default;

	void									setName(std::string name);
	const std::string&						getName() const noexcept;

private:
	Input<T>								m_input;
	Output<T>								m_output;

	static typename Output<T>::PullCallback	makePullCallback(Input<T>& input);

};

	
}

#include "DummyPad.inl"