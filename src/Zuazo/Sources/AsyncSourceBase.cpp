#include "AsyncSourceBase.h"

using namespace Zuazo::Sources;


AsyncSourceBase::AsyncSourceBase(){
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}


AsyncSourceBase::AsyncSourceBase(const Utils::Rational& rat) :
		Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::INPUT >(rat)
{
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}
/*
 * DROPPED MODIFIERS
 */

void AsyncSourceBase::setMaxDropped(u_int32_t max){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_maxDropped=max;
}


u_int32_t AsyncSourceBase::getMaxDropped() const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return m_maxDropped;
}


u_int32_t AsyncSourceBase::getDropped() const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return m_dropped;
}


void AsyncSourceBase::resetDropped(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_dropped=0;
}

/*
 * BUFFER MODIFIERS
 */

void	AsyncSourceBase::setMaxBufferSize(u_int32_t size){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_maxBufferSize=size;
}


u_int32_t AsyncSourceBase::getMaxBufferSize() const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return m_maxBufferSize;
}


u_int32_t AsyncSourceBase::getBufferSize() const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return m_buffer.size();
}


void AsyncSourceBase::flushBuffer(){
	std::lock_guard<std::mutex> lock(m_updateMutex);

	while(m_buffer.size())
		m_buffer.pop();
}


/*
 * GENERAL METHODS
 */


void AsyncSourceBase::update() const{
	if(m_buffer.size()){
		//There is at least an element at the buffer. Push it into the source
		SourceBase::push(std::move(m_buffer.front()));
		m_buffer.pop();
	}else{
		//Missing element -> dropped
		m_dropped++;
		if(m_dropped>m_maxDropped && m_maxDropped>=0){
			//More than tolerable dropped elements
			SourceBase::push();
		}
	}
}


void AsyncSourceBase::push(std::unique_ptr<const Packet> element){
	std::lock_guard<std::mutex> lock(m_updateMutex);

	if(m_buffer.size()>=m_maxBufferSize){
		//Buffer is full. Discard it
		element.reset();
	}else{
		//Insert it in the buffer
		m_buffer.emplace(std::move(element));
	}
}


void AsyncSourceBase::open(){
	SourceBase::open();
	Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::INPUT >::open();
}


void AsyncSourceBase::close(){
	Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::INPUT >::close();
	flushBuffer();
	SourceBase::close();
}
