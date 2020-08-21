#include "LockFreeQueue.h"

namespace Zuazo::Utils {

template <typename T>
inline LockFreeQueue<T>::LockFreeQueue(size_t capacity)
	: m_queue(capacity + 1)
	, m_read(0)
	, m_write(0)
{
}



template <typename T>
inline size_t LockFreeQueue<T>::capacity() const {
	return m_queue.size() - 1; // -1 as m_read == m_write means it is empty
}

template <typename T>
inline size_t LockFreeQueue<T>::size() const {
	//size() is added to avoid negative values on unsigned integers
	return (m_queue.size() + m_write.load() - m_read.load()) % m_queue.size();
}

template <typename T>
inline bool LockFreeQueue<T>::empty() const {
	const bool result = m_write.load() == m_read.load();
	assert(result == (size() == 0)); //Another way to calculate it
	return result;
}

template <typename T>
inline bool LockFreeQueue<T>::full() const {
	const bool result = nextIndex(m_write.load()) == m_read.load();
	assert(result == (size() == capacity())); //Another way to calculate it
	return result;
}



template <typename T>
inline typename LockFreeQueue<T>::const_reference LockFreeQueue<T>::front() const {
	assert(!empty());
	return m_queue[m_read.load()];
}

template <typename T>
inline typename LockFreeQueue<T>::reference LockFreeQueue<T>::front() {
	assert(!empty());
	return m_queue[m_read.load()];
}

template <typename T>
inline typename LockFreeQueue<T>::const_reference LockFreeQueue<T>::back() const {
	assert(!empty());
	return m_queue[m_write.load()];
}

template <typename T>
inline typename LockFreeQueue<T>::reference LockFreeQueue<T>::back() {
	assert(!empty());
	return m_queue[m_write.load()];
}



template <typename T>
inline void LockFreeQueue<T>::push(value_type element) {
	assert(!full());
	size_t writePos = m_write.load();

	//Try to update the stored value until it gets successfuly written
	while( !m_write.compare_exchange_weak(writePos, nextIndex(writePos)) ); 

	//A position in the array has been acquired. Write onto it
	m_queue[writePos] = std::move(element);
}

template <typename T>
inline void LockFreeQueue<T>::pop() {
	assert(!empty());
	size_t readPos = m_read.load();

	//Try to update the stored value until it gets successfuly written
	while( !m_read.compare_exchange_weak(readPos, nextIndex(readPos)) ); 

	//A position in the array has been acquired. Destroy the existing element
	m_queue[readPos] = value_type();
}



template <typename T>
inline size_t LockFreeQueue<T>::nextIndex(size_t i) const {
	return (i + 1) % m_queue.size();
}


}