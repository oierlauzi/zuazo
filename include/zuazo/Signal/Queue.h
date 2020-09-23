#pragma once 

#include "Layout.h"
#include "ProcessorLayout.h"
#include "Input.h"
#include "Output.h"

#include <queue>

namespace Zuazo::Signal {

template <typename T>
class Queue 
	: public Layout
	, public ProcessorLayout<T, T>
{
public:
	explicit Queue(std::string name);
	Queue(const Queue& other) = delete;
	~Queue() = default;

	Queue&							operator=(const Queue& other) = delete;

	void							updateInput();
	void							updateOutput();

private:
	std::queue<T>					m_queue;

	Input<T>						m_input;
	Output<T>						m_output;

};

}

#include "Queue.inl"