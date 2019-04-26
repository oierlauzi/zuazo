#pragma once

#include "Consumer.h"
#include "Source.h"
#include "../Timing.h"
#include "../ZuazoBase.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

namespace Zuazo::Stream{

template <typename T>
class Delay :
		public Stream::Source<T>,
		public Stream::Consumer<T>,
		public Timing::RegularUpdate,
		public ZuazoBase
{
public:
	Delay();
	Delay(const Timing::Duration& delay);
	Delay(const Delay& delay)=default;
	Delay(Delay&& delay)=default;
	virtual ~Delay();

	void										setDelay(const Timing::Duration& delay);
	const Timing::Duration&						getDelay();

	virtual void								update() const override;

	virtual void								open() override;
	virtual void								close() override;
private:
	struct QueueElement{
		std::shared_ptr<const T> 					element;
		Timing::TimePoint							ts;

		QueueElement(const std::shared_ptr<const T>& el) :
			element(el),
			ts(Timing::getTiming()->now())
		{
		}
	};

	Timing::Duration							m_delay;

	mutable std::queue<QueueElement>			m_queue;
};

template <typename T>
inline Delay<T>::Delay() :
	Timing::RegularUpdate(UpdatePriorities::DELAY)
{
	open();
}

template <typename T>
inline Delay<T>::Delay(const Timing::Duration& delay) :
	Timing::RegularUpdate(UpdatePriorities::DELAY),
	m_delay(delay)
{
	open();
}

template <typename T>
inline Delay<T>::~Delay(){
	close();
}

template <typename T>
inline void Delay<T>::setDelay(const Timing::Duration& delay){
	m_delay=delay;

	if(m_delay.count()==0) //Delay is disabled, empty the queue
		while(m_queue.size())
			m_queue.pop();
}

template <typename T>
inline const Zuazo::Timing::Duration& Delay<T>::getDelay(){
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
			if(m_queue.front().ts <= Timing::getTiming()->now() - m_delay){
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
	Timing::RegularUpdate::enable();
	ZuazoBase::open();
}

template <typename T>
inline void Delay<T>::close(){
	Timing::RegularUpdate::disable();
	Source<T>::reset();
	Consumer<T>::reset();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
	ZuazoBase::close();
}
}
