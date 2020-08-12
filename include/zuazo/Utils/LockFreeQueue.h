#pragma once

#include <cstddef>
#include <array>
#include <atomic>

namespace Zuazo::Utils {

template <typename T, size_t Capacity>
class LockFreeQueue {
public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	LockFreeQueue() = default;
	LockFreeQueue(const LockFreeQueue& other) = default;
	LockFreeQueue(LockFreeQueue&& other) = default;
	~LockFreeQueue() = default;

	LockFreeQueue&			operator=(const LockFreeQueue& other) = default;
	LockFreeQueue&			operator=(LockFreeQueue&& other) = default;

	size_t					size() const;
	static constexpr size_t	capacity();

	const_reference			front() const;
	reference				front();
	const_reference			back() const;
	reference				back();

	void					push(value_type element);
	void					pop();


private:
	std::array<T, Capacity>	m_queue;
	std::atomic<size_t>		m_read;
	std::atomic<size_t>		m_write;

	constexpr size_t 		nextIndex(size_t i) const;
};

}

#include "LockFreeQueue.inl"