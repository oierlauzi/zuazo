#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing/RegularUpdate.h"
#include "../Timing/TimePoint.h"
#include "../Timing/TimeInterval.h"
#include "../Timing/TimingTable.h"
#include "../Timing/UpdateOrder.h"
#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

template<typename T>
class Delay :
		public Consumer<T>,
		public Source<T>,
		public Timing::RegularUpdate<Timing::UpdateOrder::LAST>
{
public:
	Delay();
	Delay(const Timing::TimeInterval& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void								setDelay(const Timing::TimeInterval& delay);
	Timing::TimeInterval				getDelay();

	virtual void						update() const override;

	virtual void						open() override;
	virtual void						close() override;
private:
	struct QueueElement{
		std::shared_ptr<const T>			element;
		Timing::TimePoint					ts;
	};

	Timing::TimeInterval 					m_delay;

	mutable std::queue<QueueElement>		m_queue;
};

template<typename T>
inline Delay<T>::Delay() :
Timing::RegularUpdate<Timing::UpdateOrder::LAST>(){
	setDelay(Timing::TimeInterval(0));
}

template<typename T>
inline Delay<T>::Delay(const Timing::TimeInterval& delay) :
Timing::RegularUpdate<Timing::UpdateOrder::LAST>(){
	setDelay(delay);
}

template<typename T>
inline Delay<T>::~Delay(){

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
		m_queue.emplace();
		m_queue.back().element=Consumer<T>::get(&m_queue.back().ts); //A bit ugly

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
	Timing::RegularUpdate<Timing::UpdateOrder::LAST>::open();
}

template<typename T>
inline void Delay<T>::close(){
	Timing::RegularUpdate<Timing::UpdateOrder::LAST>::close();
	Source<T>::close();
	Consumer<T>::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}

}
