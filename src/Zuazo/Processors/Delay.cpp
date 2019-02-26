#include "Delay.h"

using namespace Zuazo::Processors;

Delay::Delay(){
	open();
}


Delay::Delay(const Timing::TimeInterval& delay) :
Timing::RegularUpdate< (u_int32_t)UpdateOrder::DELAY >(),
m_delay(delay)
{
	open();
}


Delay::~Delay(){
	close();
}


void Delay::setDelay(const Timing::TimeInterval& delay){
	std::lock_guard<std::mutex> lock(Updateable::m_updateMutex);
	m_delay=delay;

	if(m_delay.count()==0) //Delay is disabled, empty the queue
		while(m_queue.size())
			m_queue.pop();
}


Zuazo::Timing::TimeInterval Delay::getDelay(){
	return m_delay;
}


void Delay::update() const{
	std::shared_ptr<const Stream::Packet> lastElement;

	if(m_delay.count()){
		//The delay is active
		//Insert an element into the queue
		m_queue.emplace(ConsumerBase::get());

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
		lastElement=ConsumerBase::get();
	}
	//Push the new element
	SourceBase::push(lastElement);
}


void Delay::open(){
	SourceBase::open();
	ConsumerBase::open();
	Timing::RegularUpdate< (u_int32_t)UpdateOrder::DELAY >::open();
}


void Delay::close(){
	Timing::RegularUpdate< (u_int32_t)UpdateOrder::DELAY >::close();
	SourceBase::close();
	ConsumerBase::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
}
