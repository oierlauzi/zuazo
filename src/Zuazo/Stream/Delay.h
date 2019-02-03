#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing/TimePoint.h"
#include "../Timing/TimeUnit.h"
#include "../Timing/Timing.h"
#include "Consumer.h"
#include "LazySource.h"
#include "Source.h"

namespace Zuazo::Stream{

template<typename T>
class Delay :
		public Consumer<T>,
		public Source<T>,
		public Timing::TimingTable::RegularEvent<Timing::TimingTable::UpdatePriority::LAST>
{
public:
	Delay();
	Delay(const Timing::TimeUnit& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void								setDelay(const Timing::TimeUnit& delay);
	Timing::TimeUnit					getDelay();

	virtual void						update() override;

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
	Timing::TimingTable::RegularEvent<Timing::TimingTable::UpdatePriority::LAST>::open();
}

template<typename T>
inline void Delay<T>::close(){
	//TODO lock timing
	Timing::TimingTable::RegularEvent<Timing::TimingTable::UpdatePriority::LAST>::close();
	Source<T>::push();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}

}
