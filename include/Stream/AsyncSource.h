#pragma once

#include "Source.h"
#include "../Timing.h"
#include "../Utils/Rational.h"

#include <sys/types.h>
#include <memory>
#include <mutex>
#include <queue>

namespace Zuazo::Stream{

template <typename T>
class AsyncSource :
		public Source<T>,
		public Timing::PeriodicUpdate
{
public:
	AsyncSource();
	AsyncSource(const Utils::Rational& rat);
	AsyncSource(const AsyncSource& other)=delete;
	virtual ~AsyncSource()=default;

	void								setMaxDropped(u_int32_t max);
	u_int32_t							getMaxDropped() const;
	u_int32_t							getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(u_int32_t size);
	u_int32_t							getMaxBufferSize() const;
	u_int32_t							getBufferSize() const;
	void								flushBuffer();

	void								reset();

	virtual void 						update() const override;
protected:
	void								push(std::unique_ptr<const T> element);
private:
	static constexpr u_int32_t			DEFAULT_MAX_DROPPED=10;
	static constexpr u_int32_t			DEFAULT_MAX_BUFFER_SIZE=3;

	u_int32_t							m_maxDropped;
	mutable u_int32_t					m_dropped;
	mutable std::mutex					m_mutex;

	u_int32_t							m_maxBufferSize;
	mutable std::queue<std::unique_ptr<const T>> m_buffer;
};

template <typename T>
class AsyncSourcePad :
		public AsyncSource<T>
{
public:
	using AsyncSource<T>::push;
	using AsyncSource<T>::reset;
	using AsyncSource<T>::enable;
	using AsyncSource<T>::disable;
	using AsyncSource<T>::AsyncSource;
};

template <typename T>
AsyncSource<T>::AsyncSource() :
		Timing::PeriodicUpdate(UpdatePriorities::SOURCE)
{
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}

template <typename T>
AsyncSource<T>::AsyncSource(const Utils::Rational& rat) :
		Timing::PeriodicUpdate(UpdatePriorities::SOURCE, rat)
{
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}
/*
 * DROPPED MODIFIERS
 */

template <typename T>
inline void AsyncSource<T>::setMaxDropped(u_int32_t max){
	m_maxDropped=max;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getMaxDropped() const{
	return m_maxDropped;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getDropped() const{
	return m_dropped;
}

template <typename T>
inline void AsyncSource<T>::resetDropped(){
	m_dropped=0;
}

/*
 * BUFFER MODIFIERS
 */

template <typename T>
inline void	AsyncSource<T>::setMaxBufferSize(u_int32_t size){
	m_maxBufferSize=size;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getMaxBufferSize() const{
	return m_maxBufferSize;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getBufferSize() const{
	return m_buffer.size();
}

template <typename T>
inline void AsyncSource<T>::flushBuffer(){

	while(m_buffer.size())
		m_buffer.pop();
}


/*
 * GENERAL METHODS
 */

template <typename T>
void AsyncSource<T>::reset(){
	flushBuffer();
	Source<T>::reset();
}

template <typename T>
inline void AsyncSource<T>::update() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_buffer.size()){
		//There is at least an element at the buffer. Push it into the source
		Source<T>::push(std::move(m_buffer.front()));
		m_buffer.pop();
		m_dropped=0;
	}else{
		//Missing element -> dropped
		m_dropped++;
		if(m_dropped > m_maxDropped && m_maxDropped >= 0){
			//More than tolerable dropped elements
			Source<T>::push();
		}
	}
}


template <typename T>
inline void AsyncSource<T>::push(std::unique_ptr<const T> element){
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_buffer.size()>=m_maxBufferSize){
		//Buffer is full. Discard it
		element.reset();
	}else{
		//Insert it in the buffer
		m_buffer.emplace(std::move(element));
	}
}

}
