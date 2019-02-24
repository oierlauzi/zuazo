#pragma once

#include <memory>
#include <mutex>
#include <set>

#include "../Timing/TimePoint.h"
#include "../Timing/Timing.h"
#include "../Updateable.h"
#include "Consumer.h"


namespace Zuazo::Stream{

template <typename T>
class Consumer;

template <typename T>
class CallableConsumer;

template <typename T>
class Source :
		public virtual Updateable
{
	friend Consumer<T>;
public:
	Source()=default;
	Source(const Source<T>& other)=delete;
	virtual ~Source();

	virtual std::shared_ptr<const T>	get() const;
protected:
	void								push() const;
	void								push(std::shared_ptr<const T>& element) const;
	void								push(std::unique_ptr<const T> element) const;

	virtual void						open() override;
	virtual void						close() override;
private:
	mutable std::shared_ptr<const T>	m_last;

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
void Source<T>::push() const{
	m_last=std::shared_ptr<const T>();
}

template <typename T>
void Source<T>::push(std::unique_ptr<const T> element) const{
	m_last=std::move(element);
}

template <typename T>
void Source<T>::push(std::shared_ptr<const T>& element) const{
	m_last=element;
}

template <typename T>
void Source<T>::open(){
	Updateable::open();
}

template <typename T>
void Source<T>::close(){
	m_last=std::shared_ptr<const T>();
	Updateable::close();
}

template <typename T>
void Source<T>::attach(Consumer<T> * cons) const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_consumers.insert(cons);
}

template <typename T>
void Source<T>::detach(Consumer<T> * cons) const{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_consumers.erase(cons);
}
} /* namespace Zuazo */
