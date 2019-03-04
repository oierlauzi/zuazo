#pragma once

#include "Delay.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing.h"
#include "../Updateables/RegularUpdate.h"
#include "../Updateables/UpdateOrder.h"
#include "../Utils/TimeInterval.h"
#include "../Utils/TimePoint.h"
#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class Delay :
		public Stream::Source<T>,
		public Stream::Consumer<T>,
		public Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DELAY>
{
public:
	Delay();
	Delay(const Utils::TimeInterval& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void										setDelay(const Utils::TimeInterval& delay);
	Utils::TimeInterval							getDelay();

	virtual void								update() const override;

	virtual void								open() override;
	virtual void								close() override;
private:
	struct QueueElement{
		std::shared_ptr<const T> 					element;
		Utils::TimePoint							ts;

		QueueElement(const std::shared_ptr<const T>& el) :
			element(el),
			ts(timings->now())
		{
		}
	};

	Utils::TimeInterval 						m_delay;

	mutable std::queue<QueueElement>			m_queue;
};

template <typename T>
inline Delay<T>::Delay(){
	open();
}

template <typename T>
inline Delay<T>::Delay(const Utils::TimeInterval& delay) :
	Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DELAY>(),
	m_delay(delay)
{
	open();
}

template <typename T>
inline Delay<T>::~Delay(){
	close();
}

template <typename T>
inline void Delay<T>::setDelay(const Utils::TimeInterval& delay){
	std::lock_guard<std::mutex> lock(Updateable::m_updateMutex);
	m_delay=delay;

	if(m_delay.count()==0) //Delay is disabled, empty the queue
		while(m_queue.size())
			m_queue.pop();
}

template <typename T>
inline Zuazo::Utils::TimeInterval Delay<T>::getDelay(){
	return m_delay;
}

template <typename T>
inline void Delay<T>::update() const{
	std::shared_ptr<const T> lastElement; //TODO refresh this code using Consumer::hasChanged()

	if(m_delay.count()){
		//The delay is active
		//Insert an element into the queue
		m_queue.emplace(Consumer<T>::get());

		//Advance the queue until an element with the desired time-stamp is found
		while(m_queue.size()){
			if(m_queue.front().ts <= timings->now() - m_delay){
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

template <typename T>
inline void Delay<T>::open(){
	Source<T>::open();
	Consumer<T>::open();
	Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DELAY>::open();
}

template <typename T>
inline void Delay<T>::close(){
	Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DELAY>::close();
	Source<T>::close();
	Consumer<T>::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}
}
