#pragma once 

#include "BinomialLayout.h"

#include <queue>

namespace Zuazo::Signal {

template <typename T>
class Queue 
	: public BinomialLayout<T>
{
public:
	explicit Queue(std::string name);
	Queue(const Queue& other) = delete;
	Queue(Queue&& other) = default;
	~Queue() = default;

	Queue&							operator=(const Queue& other) = delete;
	Queue&							operator=(Queue&& other) = default;

	void							updateInput();
	void							updateOutput();

private:
	std::queue<T>					m_queue;

};

}

#include "Queue.inl"