#pragma once

#include <sys/types.h>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing.h"
#include "../Utils/Rational.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class AsyncSource : public Source<T>, public Timing::Periodic<Timing::UpdatePriority::INPUT>{
public:
	AsyncSource();
	AsyncSource(const Utils::Rational& rat);
	AsyncSource(const AsyncSource<T>& other)=delete;
	virtual ~AsyncSource();

	void								setMaxDropped(u_int32_t max);
	u_int32_t							getMaxDropped() const;
	u_int32_t							getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(u_int32_t size);
	u_int32_t							getMaxBufferSize() const;
	u_int32_t							getBufferSize() const;
	void								flushBuffer();

	virtual void						open() override;
	virtual void						close() override;
protected:
	void								push(std::unique_ptr<const T>& element);

	virtual void 						update() override;
private:
	static const u_int32_t				DEFAULT_MAX_DROPPED=3;
	static const u_int32_t				DEFAULT_MAX_BUFFER_SIZE=3;

	u_int32_t							m_maxDropped;
	u_int32_t							m_dropped;

	u_int32_t							m_maxBufferSize;
	std::queue<std::unique_ptr<const T>> m_buffer;
};

/************************
 * METHOD DEFINITIONS	*
 ************************/
/*
 * CONSTRUCTOR AND DESTRUCTOR
 */

template <typename T>
inline AsyncSource<T>::AsyncSource() : Updateable(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}

template <typename T>
inline AsyncSource<T>::AsyncSource(const Utils::Rational& rat) : Timing::Periodic<Timing::UpdatePriority::INPUT>(rat){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}

template <typename T>
inline AsyncSource<T>::~AsyncSource(){
	close();
}

/*
 * DROPPED MODIFIERS
 */
template <typename T>
inline void	AsyncSource<T>::setMaxDropped(u_int32_t max){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxDropped=max;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getMaxDropped() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_maxDropped;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getDropped() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_dropped;
}

template <typename T>
inline void AsyncSource<T>::resetDropped(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_dropped=0;
}

/*
 * BUFFER MODIFIERS
 */
template <typename T>
inline void	AsyncSource<T>::setMaxBufferSize(u_int32_t size){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxBufferSize=size;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getMaxBufferSize() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_maxBufferSize;
}

template <typename T>
inline u_int32_t AsyncSource<T>::getBufferSize() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_buffer.size();
}

template <typename T>
inline void AsyncSource<T>::flushBuffer(){
	std::lock_guard<std::mutex> lock(m_mutex);

	while(m_buffer.size())
		m_buffer.pop();
}


/*
 * GENERAL METHODS
 */

template <typename T>
void AsyncSource<T>::update() {
	if(m_buffer.size()){
		//There is at least an element at the buffer. Push it into the source
		Source<T>::push(m_buffer.front());
		m_buffer.pop();
	}else{
		//Missing element -> dropped
		m_dropped++;
		if(m_dropped>m_maxDropped && m_maxDropped>=0){
			//More than tolerable dropped elements
			Source<T>::push();
		}
	}
}

template <typename T>
void AsyncSource<T>::push(std::unique_ptr<const T>& element){
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_maxBufferSize>0){
		//We are buffering the source
		if(m_buffer.size()>=m_maxBufferSize){
			//Buffer is full. Discard it
			element.reset();
		}else{
			//Insert it in the buffer
			m_buffer.emplace(std::move(element));
		}
	}else{
		//The source is not being buffered, simply push it
		Source<T>::push(m_buffer.front());
		m_dropped=0;
	}
}

template <typename T>
void AsyncSource<T>::open(){
	Timing::Periodic<Timing::UpdatePriority::INPUT>::open();
}

template <typename T>
void AsyncSource<T>::close(){
	Timing::Periodic<Timing::UpdatePriority::INPUT>::close();
	flushBuffer();
	Source<T>::push();
}
}
