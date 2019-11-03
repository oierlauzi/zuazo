#include "AsyncOutput.h"
#include "../Timing/MainLoop.h"

namespace Zuazo::Signal {

template <typename T>
inline void AsyncOutput<T>::setMaxDropped(int32_t max){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxDropped = max;
}

template <typename T>
inline int32_t AsyncOutput<T>::getMaxDropped() const{
	return m_maxDropped;
}

template <typename T>
inline uint32_t AsyncOutput<T>::getDropped() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_dropped;
}

template <typename T>
inline void AsyncOutput<T>::resetDropped(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_dropped = 0;
}


template <typename T>
inline void	AsyncOutput<T>::setMaxBufferSize(int32_t size){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxBufferSize = size;
}

template <typename T>
inline int32_t AsyncOutput<T>::getMaxBufferSize() const{
	return m_maxBufferSize;
}

template <typename T>
inline uint32_t AsyncOutput<T>::getBufferSize() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_buffer.size();
}

template <typename T>
inline void AsyncOutput<T>::flushBuffer(){
	std::lock_guard<std::mutex> lock(m_mutex);

	while(m_buffer.size()){
		m_buffer.pop();
	}
}

template <typename T>
inline void AsyncOutput<T>::reset(){
	flushBuffer();
	Output<T>::reset();
}

template <typename T>
inline void AsyncOutput<T>::update() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_buffer.size()){
		//There is at least an element at the buffer. Push it into the output
		Output<T>::push(std::move(m_buffer.front()));
		m_buffer.pop();
		m_dropped=0;
	}else{
		//Missing element -> dropped
		m_dropped++;
		if(m_dropped > m_maxDropped && m_maxDropped >= 0){
			//More than tolerable dropped elements
			Output<T>::push(std::shared_ptr<const T>());
		}
	}
}


template <typename T>
inline void AsyncOutput<T>::push(std::shared_ptr<const T>&& element){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_buffer.size() < m_maxBufferSize && m_maxBufferSize >= 0){
		//Insert it in the buffer
		m_buffer.emplace(std::forward<std::shared_ptr<const T>>(element));
	} //else There are too many elements. Discard
}

}