#pragma once

#include <memory>
#include <mutex>
#include <set>

#include "../Timing/TimePoint.h"
#include "../Timing/Timing.h"
#include "Consumer.h"


namespace Zuazo::Stream{

template <typename T>
class Consumer;

template <typename T>
class CallableConsumer;

template <typename T>
class Source{
	friend Consumer<T>;
public:
	Source()=default;
	Source(const Source<T>& other)=delete;
	virtual ~Source();

	virtual std::shared_ptr<const T>	get() const;
	virtual std::shared_ptr<const T>	get(Timing::TimePoint* ts) const;
protected:
	void								push();
	void								push(std::shared_ptr<const T>& element);
	void								push(std::unique_ptr<const T>& element);

	bool								isActive() const;
private:
	mutable std::mutex					m_mutex;

	std::shared_ptr<const T>			m_last;
	Timing::TimePoint					m_elementTs;

	mutable std::set<Consumer<T> *>		m_consumers;

	void								attach(Consumer<T> * cons) const;
	void								detach(Consumer<T> * cons) const;
};

/********************************
 * METHOD DEFINITIONS	*
 ********************************/
/*
 * CONSTRUCTOR AND DESTRUCTOR
 */

template <typename T>
inline Source<T>::~Source(){
	while(m_consumers.size())
		(*m_consumers.begin())->setSource(nullptr);
}

/*
 * GENERAL METHODS
 */

template <typename T>
std::shared_ptr<const T> Source<T>::get() const{
	return m_last;
}

template <typename T>
std::shared_ptr<const T> Source<T>::get(Timing::TimePoint* ts) const{
	*ts=m_elementTs;
	return m_last;
}

template <typename T>
void Source<T>::push(){
	std::shared_ptr<const T> newEl;
	push(newEl);
}

template <typename T>
void Source<T>::push(std::unique_ptr<const T>& element){
	std::shared_ptr<const T> newEl=std::move(element);
	push(newEl);
}

template <typename T>
void Source<T>::push(std::shared_ptr<const T>& element){
	m_last=element;
	m_elementTs=Timing::timings->now();
}

template <typename T>
void Source<T>::attach(Consumer<T> * cons) const{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_consumers.insert(cons);
}

template <typename T>
void Source<T>::detach(Consumer<T> * cons) const{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_consumers.erase(cons);
}

/*
 * CONSUMER RELATED THINGS
 */

template <typename T>
bool Source<T>::isActive() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	for(Consumer<T> * cons : m_consumers)
		if(cons->isActive())
			return true;

	return false;
}
} /* namespace Zuazo */
