#include "AsyncOutput.h"
#include "../Timing/MainLoop.h"

namespace Zuazo::Signal {

template <typename T>
inline void AsyncOutput<T>::setMaxDropped(int max){
	m_maxDropped = max;
}

template <typename T>
inline int AsyncOutput<T>::getMaxDropped() const{
	return m_maxDropped;
}

template <typename T>
inline uint AsyncOutput<T>::getDropped() const{
	return m_dropped;
}

template <typename T>
inline void AsyncOutput<T>::resetDropped(){
	m_dropped;
}


template <typename T>
inline void	AsyncOutput<T>::setMaxBufferSize(size_t size){
	std::lock_guard<std::mutex> lock(m_mutex); //Lock to prevent any new elements being added
	m_buffer.resize(size + 1);
}

template <typename T>
inline size_t AsyncOutput<T>::getMaxBufferSize() const{
	return m_buffer.size();
}

template <typename T>
inline size_t AsyncOutput<T>::getBufferSize() const{
	const size_t read = m_read.load();
	const size_t write = m_write.load();
	const size_t bufSize = m_buffer.size();

	return (write + bufSize - read) % bufSize;
}

template <typename T>
inline void AsyncOutput<T>::flushBuffer(){
	size_t read, write;
	while((read = m_read.load()) !=  (write = m_write.load())){
		m_buffer[read].reset();
		m_read.store(getNextValue(read));
	}
}

template <typename T>
inline void AsyncOutput<T>::update() {
	//No need for lock buffer resizing. 
	//update() shouldnt be called while there is an active context

	const size_t read = m_read.load();
	const size_t write = m_write.load();
	const size_t nextRead = getNextValue(read);

	if(read != write) {
		//There is at least 1 element in the buffer
		m_dropped = 0;
		Output<T>::push(std::move(m_buffer[read]));
		m_read.store(nextRead);
	} else {
		//Missing element -> increment the dropped counter
		m_dropped++;

		if(m_dropped > m_maxDropped && m_maxDropped >= 0){
			//More than tolerable dropped elements
			Output<T>::push(T());
		}
	}
}

template <typename T>
inline void AsyncOutput<T>::reset(){
	flushBuffer();
	Output<T>::reset();
}


template <typename T>
inline void AsyncOutput<T>::push(T&& element){
	std::lock_guard<std::mutex> lock(m_mutex); //Lock if it is being resized.

	const size_t read = m_read.load();
	const size_t write = m_write.load();
	const size_t nextWrite =  getNextValue(write);

	if(read != nextWrite) {
		m_buffer[write] = std::move(element);
		m_write.store(nextWrite);
	} //else discard element
}

template <typename T>
inline size_t AsyncOutput<T>::getNextValue(size_t i) const{
	return (i + 1) % m_buffer.size();
}

}