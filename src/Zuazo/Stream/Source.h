#pragma once

#include <sys/types.h>
#include <memory>
#include <queue>
#include <set>

#include "../Utils/Rational.h"
#include "../Timing.h"
#include "Consumer.h"

namespace Zuazo::Stream{

template <typename T>
class Consumer;

template <typename T>
class Source : public Timing::Updateable{
	friend Consumer<T>;
public:
	Source();
	Source(const Rational& rat);
	Source(const Source<T>& other)=delete;
	virtual ~Source();

	void								setMaxBufferSize(u_int32_t size);
	u_int32_t							getMaxBufferSize() const;
	u_int32_t							getBufferSize() const;
	void								flushBuffer();

	void								setMaxDropped(u_int32_t max);
	u_int32_t							getMaxDropped() const;
	u_int32_t							getDropped() const;
	void								resetDropped();

	virtual void 						update(const Timing::time_unit& elapsed, const Timing::time_point& ts) override;
	virtual std::shared_ptr<const T>	get() const;
protected:
	void								push(std::unique_ptr<T>& element);

	virtual void						open() override;
	virtual void						close() override;

	bool								isActive() const;
private:
	static const u_int32_t				DEFAULT_MAX_BUFFER_SIZE=2;
	static const u_int32_t				DEFAULT_MAX_DROPPED=3;

	u_int32_t							m_maxBufferSize;
	std::queue<std::unique_ptr<T>>		m_buffer;

	u_int32_t							m_maxDropped;
	u_int32_t							m_dropped;
	std::shared_ptr<T>					m_last;

	std::set<Consumer<T> *>				m_consumers;
};

/************************
 * FUNCTION DEFINITIONS	*
 ************************/
/*
 * CONSTRUCTOR AND DESTRUCTOR
 */

template <typename T>
inline Source<T>::Source() : Timing::Updateable(){
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}

template <typename T>
inline Source<T>::Source(const Rational& rat) : Timing::Updateable(rat){
	m_maxBufferSize=DEFAULT_MAX_BUFFER_SIZE;
	m_maxDropped=DEFAULT_MAX_DROPPED;
	m_dropped=0;
}

template <typename T>
inline Source<T>::~Source(){
	for(Consumer<T> * cons : m_consumers)
		cons->setSource(nullptr);
}

/*
 * BUFFER MODIFIERS
 */
template <typename T>
inline void	Source<T>::setMaxBufferSize(u_int32_t size){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_maxBufferSize=size;
}

template <typename T>
inline u_int32_t Source<T>::getMaxBufferSize() const{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_maxBufferSize;
}

template <typename T>
inline u_int32_t Source<T>::getBufferSize() const{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_buffer.size();
}

template <typename T>
inline void Source<T>::flushBuffer(){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	while(m_buffer.size())
		m_buffer.pop();
}

/*
 * DROPPED MODIFIERS
 */
template <typename T>
inline void	Source<T>::setMaxDropped(u_int32_t max){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_maxDropped=max;
}

template <typename T>
inline u_int32_t Source<T>::getMaxDropped() const{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_maxDropped;
}

template <typename T>
inline u_int32_t Source<T>::getDropped() const{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_dropped;
}

template <typename T>
inline void Source<T>::resetDropped(){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_dropped=0;
}

/*
 * SOURCE RELATED THINGS
 */

template <typename T>
void Source<T>::update(const Timing::time_unit& elapsed, const Timing::time_point& ts) {
	//std::lock_guard<std::recursive_mutex> lock(m_mutex); //Not needed, already locked
	if(m_buffer.size()){
		m_last=std::move(m_buffer.front());
		m_buffer.pop();
	}else{
		m_dropped++;
		if(m_dropped>m_maxDropped)
			m_last=std::shared_ptr<T>();
	}
}

template <typename T>
std::shared_ptr<const T> Source<T>::get() const{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_last;
}

template <typename T>
void Source<T>::push(std::unique_ptr<T>& element){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if(m_maxBufferSize>0){
		if(m_buffer.size()>=m_maxBufferSize)
			element.reset();
		else
			m_buffer.emplace(std::move(element));
	}else{
		m_last=std::move(element);
		m_dropped=0;
	}
}

template <typename T>
void Source<T>::open(){
	Timing::Updateable::open();
}

template <typename T>
void Source<T>::close(){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	flushBuffer();
	m_last=std::shared_ptr<T>();
}

/*
 * CONSUMER RELATED THINGS
 */

template <typename T>
bool Source<T>::isActive() const{
	for(Consumer<T> * cons : m_consumers)
		if(cons->isActive())
			return true;

	return false;
}
} /* namespace Zuazo */
