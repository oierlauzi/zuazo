#pragma once 

#include "Layout.h"
#include "Input.h"
#include "../Utils/Pimpl.h"
#include "../Utils/LockFreeQueue.h"

namespace Zuazo::Signal {

template <typename T>
class AsyncInput 
	: public Layout
{
public:
	AsyncInput(std::string name);
	AsyncInput(const DummyPad& other) = delete;
	AsyncInput(DummyPad&& other) = default;
	~AsyncInput() = default;

	AsyncInput&							operator=(const AsyncInput& other) = delete;
	AsyncInput&							operator=(AsyncInput&& other) = default;

	PadProxy<Input<T>>&					getInput();
	const PadProxy<Input<T>>&			getInput() const;

	size_t								getCapacity() const;
	void								setMaxDelay(size_t maxDelay);
	size_t								getMaxDelay() const;
	size_t								getDelay() const;

	T									pull();
	void								update();

private:
	//Dynamically allocated so that its address remains invariant
	Utils::Pimpl<Input<T>>			m_input;
	Utils::LockFreeQueue<T, 64>		m_queue;
	size_t							m_maxDelay;

};

	
}

#include "AsyncInput.inl"