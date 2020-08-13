#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "../Chrono.h"
#include "../Utils/Pimpl.h"

namespace Zuazo::Signal {

template <typename T>
class Delay 
	: public Layout
{
public:
	Delay(std::string name, Duration delay);
	Delay(const DummyPad& other) = delete;
	Delay(DummyPad&& other) = default;
	~Delay() = default;

	Delay&								operator=(const Delay& other) = delete;
	Delay&								operator=(Delay&& other) = default;

	PadProxy<Input<T>>&					getInput();
	const PadProxy<Input<T>>&			getInput() const;
	PadProxy<Output<T>>&				getOutput();
	const PadProxy<Output<T>>&			getOutput() const;

	void								setDelay(Duration delay);
	Duration							getDelay() const;

	void								update(TimePoint now);

private:
	struct IO {
		Input<T> input = Input<T>("in");
		Output<T> output = Output<T>("out");
	};

	struct Packet {
		TimePoint time;
		T data;
	};

	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<IO>				m_io; 
	Duration						m_delay;
	std::deque<Packet>				m_delayLine;

};

	
}

#include "Delay.inl"