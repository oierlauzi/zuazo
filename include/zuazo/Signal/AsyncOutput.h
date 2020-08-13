#pragma once 

#include "Layout.h"
#include "Output.h"
#include "../Utils/Pimpl.h"
#include "../Utils/LockFreeQueue.h"

namespace Zuazo::Signal {

template <typename T>
class AsyncOutput 
	: public Layout
{
public:
	AsyncOutput(std::string name);
	AsyncOutput(const DummyPad& other) = delete;
	AsyncOutput(DummyPad&& other) = default;
	~AsyncOutput() = default;

	AsyncOutput&					operator=(const AsyncOutput& other) = delete;
	AsyncOutput&					operator=(AsyncOutput&& other) = default;

	PadProxy<Output<T>>&			getOutput();
	const PadProxy<Output<T>>&		getOutput() const;

	size_t							getCapacity() const;
	void							setMaxDelay(size_t maxDelay);
	size_t							getMaxDelay() const;
	size_t							getDelay() const;

	void							setMaxDropped(size_t maxDropped);
	size_t							getMaxDropped() const;
	size_t							getDropped() const;

	void							push(T element);
	void							update();

private:
	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<Output<T>>			m_output;
	Utils::LockFreeQueue<T, 64>		m_queue;
	size_t							m_maxDelay;
	size_t							m_maxDropped;
	size_t							m_dropped;

};

	
}

#include "AsyncOutput.inl"