#pragma once 

#include "BinomialLayout.h"
#include "Input.h"
#include "Output.h"
#include "../Utils/LockFreeQueue.h"

#include <atomic>

namespace Zuazo::Signal {

template <typename T>
class Synchronizer 
	: public BinomialLayout
{
public:
	explicit Synchronizer(std::string name, size_t maxDelay = 3, size_t maxDropped = 1);
	Synchronizer(const Synchronizer& other) = delete;
	Synchronizer(Synchronizer&& other) = default;
	~Synchronizer() = default;

	Synchronizer&					operator=(const Synchronizer& other) = delete;
	Synchronizer&					operator=(Synchronizer&& other) = default;

	size_t							getMaxDelay() const;
	size_t							getDelay() const;

	size_t							getMaxDropped() const;
	size_t							getDropped() const;

	void							updateInput();
	void							updateOutput();

private:
	Utils::LockFreeQueue<T>			m_queue;
	size_t							m_maxDropped;
	std::atomic<size_t>				m_dropped;

};

}

#include "Synchronizer.inl"