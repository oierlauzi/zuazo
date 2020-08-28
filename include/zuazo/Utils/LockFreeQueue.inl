#include "LockFreeQueue.h"

#include <cassert>

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
	m_queue[advance(m_write)] = std::move(element);
}

template <typename T>
inline void LockFreeQueue<T>::pop() {
	assert(!empty());
	m_queue[advance(m_read)] = value_type();
}



template <typename T>
inline size_t LockFreeQueue<T>::nextIndex(size_t i) const {
	return (i + 1) % m_queue.size();
}

template <typename T>
inline size_t LockFreeQueue<T>::advance(std::atomic<size_t>& index) const {
	size_t value = index.load();

	//Try to update the stored value until it gets successfuly written
	while( !index.compare_exchange_weak(value, nextIndex(value)) ); //FIXME might be better to use strong comparisons

	return value;
}


}