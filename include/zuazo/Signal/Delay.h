#pragma once 

#include "Layout.h"
#include "ProcessorLayout.h"
#include "Input.h"
#include "Output.h"
#include "../Chrono.h"

#include <deque>

namespace Zuazo::Signal {

template <typename T>
class Delay 
	: public Layout
	, public ProcessorLayout<T, T>
{
public:
	Delay(std::string name, Duration delay);
	Delay(const Delay& other) = delete;
	~Delay() = default;

	Delay&							operator=(const Delay& other) = delete;

	void							setDelay(Duration delay) noexcept;
	Duration						getDelay() const noexcept;

	void							update(TimePoint now);

private:
	struct Packet {
		TimePoint time;
		T data;
	};

	Duration						m_delay;
	std::deque<Packet>				m_delayLine;

	Input<T>						m_input;
	Output<T>						m_output;

};

	
}

#include "Delay.inl"