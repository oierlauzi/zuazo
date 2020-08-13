#include "LockFreeQueue.h"

namespace Zuazo::Utils {

template <typename T, size_t Capacity>
inline size_t LockFreeQueue<T, Capacity>::size() const {
	//size() is added to avoid negative values on unsigned integers
	return (m_queue.size() + m_write.load() - m_read.load()) % m_queue.size();
}

template <typename T, size_t Capacity>
constexpr size_t LockFreeQueue<T, Capacity>::capacity() {
	return Capacity - 1; // -1 as m_read == m_write means it is empty
}

template <typename T, size_t Capacity>
inline typename LockFreeQueue<T, Capacity>::const_reference LockFreeQueue<T, Capacity>::front() const {
	assert(size() > 0);
	return m_queue[m_read.load()];
}

template <typename T, size_t Capacity>
inline typename LockFreeQueue<T, Capacity>::reference LockFreeQueue<T, Capacity>::front() {
	assert(size() > 0);
	return m_queue[m_read.load()];
}

template <typename T, size_t Capacity>
inline typename LockFreeQueue<T, Capacity>::const_reference LockFreeQueue<T, Capacity>::back() const {
	assert(size() > 0);
	return m_queue[m_write.load()];
}

template <typename T, size_t Capacity>
inline typename LockFreeQueue<T, Capacity>::reference LockFreeQueue<T, Capacity>::back() {
	assert(size() > 0);
	return m_queue[m_write.load()];
}

template <typename T, size_t Capacity>
inline void LockFreeQueue<T, Capacity>::push(value_type element) {
	assert(size() < capacity());
	size_t writePos = m_write.load();

	//Try to update the stored value until it gets successfuly written
	while( !m_write.compare_exchange_weak(writePos, nextIndex(writePos)) ); 

	//A position in the array has been acquired. Write onto it
	m_queue[writePos] = std::move(element);
}

template <typename T, size_t Capacity>
inline void LockFreeQueue<T, Capacity>::pop() {
	assert(size() > 0);
	size_t readPos = m_read.load();

	//Try to update the stored value until it gets successfuly written
	while( !m_read.compare_exchange_weak(readPos, nextIndex(readPos)) ); 

	//A position in the array has been acquired. Destroy the existing element
	m_queue[readPos] = value_type();
}


template <typename T, size_t Capacity>
constexpr size_t LockFreeQueue<T, Capacity>::nextIndex(size_t i) const {
	return (i + 1) % m_queue.size();
}


}