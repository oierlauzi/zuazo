#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing.h"
#include "CallableConsumer.h"
#include "Consumer.h"
#include "LazySource.h"
#include "Source.h"

namespace Zuazo::Stream{

template<typename T>
class Delay : public CallableConsumer<T>, public LazySource<T>{
public:
	Delay();
	Delay(const Timing::TimeUnit& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void								setDelay(const Timing::TimeUnit& delay);
	Timing::TimeUnit					getDelay();

	virtual void						update() override;
	virtual void						onSourceUpdate() override;

	virtual void						open() override;
	virtual void						close() override;
private:
	struct QueueElement{
		std::shared_ptr<const T>			element;
		Timing::TimePoint					ts;
	};

	Timing::TimeUnit 					m_delay;

	mutable std::queue<QueueElement>	m_queue;
};

template<typename T>
inline Delay<T>::Delay(){
	setDelay(Timing::TimeUnit(0));
}

template<typename T>
inline Delay<T>::Delay(const Timing::TimeUnit& delay){
	setDelay(delay);
}

template<typename T>
inline Delay<T>::~Delay(){
	close();
}

template<typename T>
inline void	Delay<T>::setDelay(const Timing::TimeUnit& delay){
	std::lock_guard<std::mutex> lock(Updateable::m_updateMutex);
	m_delay=delay;

	if(m_delay.count()==0) //Delay is disabled, empty the queue
		while(m_queue.size())
			m_queue.pop();
}

template<typename T>
inline Timing::TimeUnit	Delay<T>::getDelay(){
	return m_delay;
}

template<typename T>
inline void Delay<T>::update(){
	//Advance the queue until we arrive to the element with the desired timestamp
	std::shared_ptr<const T> lastElement;
	while(m_queue.size()){
		if(m_queue.front().ts <= Timing::now() - m_delay){
			lastElement=m_queue.front().element;
			m_queue.pop();
		}else
			break;
	}

	if(lastElement)
		Source<T>::push(lastElement);
}

template<typename T>
inline void Delay<T>::onSourceUpdate(){
	std::lock_guard<std::mutex> lock(Updateable::m_updateMutex);

	//Insert new element on the queue
	m_queue.emplace();
	QueueElement& newEl=m_queue.back();
	newEl.element=CallableConsumer<T>::get(&newEl.ts);
	update();
}

template<typename T>
inline void Delay<T>::open(){
	LazySource<T>::open();
}

template<typename T>
inline void Delay<T>::close(){
	LazySource<T>::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}

}
