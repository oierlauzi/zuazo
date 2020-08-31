#pragma once 

#include "BinomialLayout.h"
#include "Input.h"
#include "Output.h"
#include "../Chrono.h"

#include <deque>

namespace Zuazo::Signal {

template <typename T>
class Delay 
	: public BinomialLayout<T>
{
public:
	Delay(std::string name, Duration delay);
	Delay(const Delay& other) = delete;
	Delay(Delay&& other) = default;
	~Delay() = default;

	Delay&							operator=(const Delay& other) = delete;
	Delay&							operator=(Delay&& other) = default;

	void							setDelay(Duration delay);
	Duration						getDelay() const;

	void							update(TimePoint now);

private:
	struct Packet {
		TimePoint time;
		T data;
	};

	Duration						m_delay;
	std::deque<Packet>				m_delayLine;

};

	
}

#include "Delay.inl"