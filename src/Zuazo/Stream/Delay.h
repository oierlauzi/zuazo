#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing.h"
#include "../Updateable.h"
#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

template<typename T>
class Delay : public Consumer<T>, public Source<T>, public Updateable<UpdatePriority::DONT_CARE>{
public:
	Delay();
	Delay(const Timing::TimeUnit& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void								setDelay(const Timing::TimeUnit& delay);
	Timing::TimeUnit					getDelay();

	virtual std::shared_ptr<const T>	get() const override;
	virtual std::shared_ptr<const T>	get(Timing::TimePoint* ts) const override;

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
	mutable QueueElement				m_lastElement;
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
	std::lock_guard<std::mutex> lock(m_mutex);
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
inline std::shared_ptr<const T> Delay<T>::get() const{
	const_cast<Delay<T>*> (this)->perform();
	return m_lastElement.element;
}

template<typename T>
inline std::shared_ptr<const T> Delay<T>::get(Timing::TimePoint* ts) const{
	const_cast<Delay<T>*> (this)->perform();
	*ts=m_lastElement.ts;
	return m_lastElement.element;
}

template<typename T>
inline void Delay<T>::update(){
	if(m_delay.count()>0){
		//Insert new element on the queue
		m_queue.emplace();
		QueueElement& newEl=m_queue.back();
		newEl.element=Consumer<T>::get(&newEl.ts);

		//Advance the queue until we arrive to the element with the desired timestamp
		while(m_queue.front().ts <= Timing::now() - m_delay){
			m_lastElement=m_queue.front();
			m_queue.pop();
		}
	}else{
		//Simply forward the element
		m_lastElement.element=Consumer<T>::get(&m_lastElement.ts);
	}
}

template<typename T>
inline void Delay<T>::open(){
	Updateable<UpdatePriority::DONT_CARE>::open();
}

template<typename T>
inline void Delay<T>::close(){
	Updateable<UpdatePriority::DONT_CARE>::close();

	//Reset all
	while(m_queue.size())
		m_queue.pop();
	m_lastElement=QueueElement();
}

}
