#pragma once 

#include "Layout.h"
#include "ProcessorLayout.h"
#include "Input.h"
#include "Output.h"
#include "../Utils/LockFreeQueue.h"

#include <atomic>

namespace Zuazo::Signal {

template <typename T>
class Synchronizer 
	: public Layout
	, public ProcessorLayout<T, T>
{
public:
	explicit Synchronizer(std::string name, size_t maxDelay = 3, size_t maxDropped = 1);
	Synchronizer(const Synchronizer& other) = delete;
	~Synchronizer() = default;

	Synchronizer&					operator=(const Synchronizer& other) = delete;

	size_t							getMaxDelay() const noexcept;
	size_t							getDelay() const noexcept;

	size_t							getMaxDropped() const noexcept;
	size_t							getDropped() const noexcept;

	void							updateInput();
	void							updateOutput();

private:
	Utils::LockFreeQueue<T>			m_queue;
	size_t							m_maxDropped;
	std::atomic<size_t>				m_dropped;

	Input<T>						m_input;
	Output<T>						m_output;

};

}

#include "Synchronizer.inl"