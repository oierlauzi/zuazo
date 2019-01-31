#pragma once

#include <mutex>

#include "Timing.h"
#include "Utils/Rational.h"

namespace Zuazo{

class Timing;

/*
 * Class which defines a periodical event
 */
template <UpdatePriority TPriority>
class Updateable{
	friend Timing;
public:
	/*
	 * An update event's callback function
	 */
	class Callback{
	public:
		virtual void						update()=0;
	};

	Updateable();
	Updateable(const Utils::Rational& rate);
	Updateable(const Updateable& other);
	virtual ~Updateable();
	void								setBeforeUpdateCallback(Callback * cbk);
	void								setAfterUpdateCallback(Callback * cbk);

	const Utils::Rational& 				getInterval() const;
	Utils::Rational						getRate() const;

	void								lock() const;
	void								unlock() const;

	virtual void						open();
	virtual void						close();
	bool								isOpen() const;
protected:
	mutable std::mutex					m_mutex;

	void 								setInterval(const Utils::Rational& interval);
	void								setRate(const Utils::Rational& rate);

	virtual void						update()=0;
	virtual void						perform();
private:
	Utils::Rational						m_updateInterval;
	bool								m_isOpen;
	mutable std::mutex					m_updateMutex;

	Callback *							m_beforeCbk;
	Callback *							m_afterCbk;
};

/*
 * METHOD DEFINITIONS
 */

template <UpdatePriority TPriority>
inline Updateable<TPriority>::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;

	setRate(0);
}

template <UpdatePriority TPriority>
inline Updateable<TPriority>::Updateable(const Utils::Rational& rate){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;

	setRate(rate);
}

template <UpdatePriority TPriority>
inline Updateable<TPriority>::Updateable(const Updateable& other){
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
	m_isOpen=other.m_isOpen;

	setInterval(other.m_updateInterval);
}

template <UpdatePriority TPriority>
inline Updateable<TPriority>::~Updateable(){
	Timing::deleteTiming(this);
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::setBeforeUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_beforeCbk=cbk;
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::setAfterUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_afterCbk=cbk;
}

template <UpdatePriority TPriority>
inline const Utils::Rational& Updateable<TPriority>::getInterval() const {
	return m_updateInterval;
}

template <UpdatePriority TPriority>
inline Utils::Rational Updateable<TPriority>::getRate() const {
	return 1/m_updateInterval;
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::lock() const{
	m_updateMutex.lock();
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::unlock() const{
	m_updateMutex.unlock();
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::open(){
	if(!m_isOpen){
		m_isOpen=true;
		Timing::addTiming(this, Timing::TimeUnit(m_updateInterval));
	}
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::close(){
	if(m_isOpen){
		Timing::deleteTiming(this);
		m_isOpen=false;
	}
}

template <UpdatePriority TPriority>
inline bool Updateable<TPriority>::isOpen() const{
	return m_isOpen;
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::setInterval(const Utils::Rational& interval){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_updateInterval=interval;

	if(m_isOpen){
		Timing::TimeUnit tuInterval(m_updateInterval);
		Timing::modifyTiming(this, tuInterval);
	}
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

template <UpdatePriority TPriority>
inline void Updateable<TPriority>::perform(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	if(m_beforeCbk)
		m_beforeCbk->update();

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		update();
	}

	if(m_afterCbk)
		m_afterCbk->update();
}

}
