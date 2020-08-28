#pragma once

#include <cstddef>
#include <vector>
#include <atomic>

namespace Zuazo::Utils {

template <typename T>
class LockFreeQueue {
public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	LockFreeQueue(size_t capacity);
	LockFreeQueue(const LockFreeQueue& other) = default;
	LockFreeQueue(LockFreeQueue&& other) = default;
	~LockFreeQueue() = default;

	LockFreeQueue&			operator=(const LockFreeQueue& other) = default;
	LockFreeQueue&			operator=(LockFreeQueue&& other) = default;

	size_t					capacity() const;
	size_t					size() const;
	bool					empty() const;
	bool					full() const;

	const_reference			front() const;
	reference				front();
	const_reference			back() const;
	reference				back();

	void					push(value_type element);
	void					pop();


private:
	using Index = 

	std::vector<T>			m_queue;
	std::atomic<size_t>		m_read;
	std::atomic<size_t>		m_write;

	size_t 					nextIndex(size_t i) const;
	size_t					advance(std::atomic<size_t>& index) const;
};

}

#include "LockFreeQueue.inl"