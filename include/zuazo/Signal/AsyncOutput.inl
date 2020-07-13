#include "AsyncOutput.h"

namespace Zuazo::Signal {

/*
 * AsyncOutput
 */

template <typename T>
template<typename Str>
inline AsyncOutput<T>::AsyncOutput(	Str&& name, 
									size_t length )
	: Output<T>(std::forward<Str>(name))
	, m_buffer(length)
{
}

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
	m_dropped = 0;
}


template <typename T>
inline void AsyncOutput<T>::flushBuffer(){
	size_t read, write;
	while((read = m_read.load()) !=  (write = m_write.load())){
		m_buffer[read] = T();
		m_read.store(getNextValue(read));
	}
}

template <typename T>
inline size_t AsyncOutput<T>::getBufferSize() const {
	const size_t read = m_read.load();
	const size_t write = m_write.load();
	const size_t bufSize = m_buffer.size();

	return (write + bufSize - read) % bufSize;
}

template <typename T>
inline void AsyncOutput<T>::update() {
	const size_t read = m_read.load();
	const size_t write = m_write.load();

	if(read != write) {
		//There is at least 1 element in the buffer
		m_dropped = 0;
		Output<T>::push(std::move(m_buffer[read]));
		m_read.store(getNextValue(read));
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
template <typename Q>
inline void AsyncOutput<T>::push(Q&& element){
	const size_t read = m_read.load();
	const size_t write = m_write.load();
	const size_t nextWrite =  getNextValue(write);

	if(read != nextWrite) {
		//Write only if the buffer is not full
		m_buffer[write] = std::forward<Q>(element);
		m_write.store(nextWrite); //Advances the write head
	} //else discard element
}

template <typename T>
inline size_t AsyncOutput<T>::getNextValue(size_t i) const{
	return (i + 1) % m_buffer.size();
}

/*
 * Layout::PadProxy<AsyncOutput<T>>
 */

template <typename T>
inline Layout::PadProxy<AsyncOutput<T>>::PadProxy(AsyncOutput<T>& pad)
	: PadProxy<Output<T>>(pad)
{
}


template <typename T>
inline void Layout::PadProxy<AsyncOutput<T>>::setMaxDropped(int max) {
	PadProxy<PadBase>::template get<AsyncOutput<T>>().setMaxDropped(max);
}

template <typename T>
inline int Layout::PadProxy<AsyncOutput<T>>::getMaxDropped() const {
	return PadProxy<PadBase>::template get<AsyncOutput<T>>().getMaxDropped();
}

template <typename T>
inline uint Layout::PadProxy<AsyncOutput<T>>::getDropped() const {
	return PadProxy<PadBase>::template get<AsyncOutput<T>>().getDropped();
}

template <typename T>
inline void Layout::PadProxy<AsyncOutput<T>>::resetDropped() {
	PadProxy<PadBase>::template get<AsyncOutput<T>>().resetDropped();
}

template <typename T>
inline void Layout::PadProxy<AsyncOutput<T>>::flushBuffer() {
	PadProxy<PadBase>::template get<AsyncOutput<T>>().flushBuffer();
}

template <typename T>
inline size_t Layout::PadProxy<AsyncOutput<T>>::getBufferSize() {
	return PadProxy<PadBase>::template get<AsyncOutput<T>>().getBufferSize();
}

}