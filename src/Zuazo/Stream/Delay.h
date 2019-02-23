#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing/RegularUpdate.h"
#include "../Timing/TimePoint.h"
#include "../Timing/TimeInterval.h"
#include "../Timing/TimingTable.h"
#include "../UpdateOrder.h"
#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

template<typename T>
class Delay :
		public Consumer<T>,
		public Source<T>,
		public Timing::RegularUpdate<UpdateOrder::DELAY>
{
public:
	Delay();
	Delay(const Timing::TimeInterval& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void									setDelay(const Timing::TimeInterval& delay);
	Timing::TimeInterval					getDelay();

	virtual void							update() const override;

	virtual void							open() override;
	virtual void							close() override;
private:
	struct QueueElement{
		std::shared_ptr<const T>			element;
		Timing::TimePoint					ts;

		QueueElement(const std::shared_ptr<const T>& el) :
			element(el),
			ts(Timing::timings->now())
		{
		}
	};

	Timing::TimeInterval 					m_delay;

	mutable std::queue<QueueElement>		m_queue;
};

template<typename T>
inline Delay<T>::Delay(){
	open();
}

template<typename T>
inline Delay<T>::Delay(const Timing::TimeInterval& delay) :
Timing::RegularUpdate<UpdateOrder::DELAY>(),
m_delay(delay)
{
	open();
}

template<typename T>
inline Delay<T>::~Delay(){
	close();
}

template<typename T>
inline void	Delay<T>::setDelay(const Timing::TimeInterval& delay){
	std::lock_guard<std::mutex> lock(Updateable::m_updateMutex);
	m_delay=delay;

	if(m_delay.count()==0) //Delay is disabled, empty the queue
		while(m_queue.size())
			m_queue.pop();
}

template<typename T>
inline Timing::TimeInterval	Delay<T>::getDelay(){
	return m_delay;
}

template<typename T>
inline void Delay<T>::update() const{
	std::shared_ptr<const T> lastElement;

	if(m_delay.count()){
		//The delay is active
		//Insert an element into the queue
		m_queue.emplace(Consumer<T>::get());

		//Advance the queue until an element with the desired time-stamp is found
		while(m_queue.size()){
			if(m_queue.front().ts <= Timing::timings->now() - m_delay){
				lastElement=m_queue.front().element;
				m_queue.pop();
			}else
				break;
		}
	}else{
		//There is no delay
		lastElement=Consumer<T>::get();
	}
	//Push the new element
	Source<T>::push(lastElement);
}

template<typename T>
inline void Delay<T>::open(){
	Source<T>::open();
	Consumer<T>::open();
	Timing::RegularUpdate<UpdateOrder::DELAY>::open();
}

template<typename T>
inline void Delay<T>::close(){
	Timing::RegularUpdate<UpdateOrder::DELAY>::close();
	Source<T>::close();
	Consumer<T>::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}

}
