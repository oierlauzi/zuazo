#pragma once

#include <mutex>

#include "Timing.h"
#include "Utils/Rational.h"

namespace Zuazo{

class Timing;

/*
 * Class which defines a periodical event
 */
class Updateable{
public:
	/*
	 * An update event's callback function
	 */
	class Callback{
	public:
		virtual void						update()=0;
	};

	Updateable();
	Updateable(const Updateable& other);
	virtual ~Updateable();
	void								setBeforeUpdateCallback(Callback * cbk);
	void								setAfterUpdateCallback(Callback * cbk);

	void								lock() const;
	void								unlock() const;

	virtual void						open();
	virtual void						close();
	bool								isOpen() const;
protected:
	mutable std::mutex					m_updateMutex;

	virtual void						update()=0;
	virtual void						perform();
private:
	mutable std::mutex					m_mutex;

	bool								m_isOpen;

	Callback *							m_beforeCbk;
	Callback *							m_afterCbk;
};

/*
 * METHOD DEFINITIONS
 */


inline Updateable::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;
}


inline Updateable::Updateable(const Updateable& other){
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
	m_isOpen=other.m_isOpen;
}


inline Updateable::~Updateable(){

}


inline void Updateable::setBeforeUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_beforeCbk=cbk;
}


inline void Updateable::setAfterUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_afterCbk=cbk;
}


inline void Updateable::lock() const{
	m_mutex.lock();
}


inline void Updateable::unlock() const{
	m_mutex.unlock();
}


inline void Updateable::open(){
	m_isOpen=true;
}


inline void Updateable::close(){
	m_isOpen=false;
}


inline bool Updateable::isOpen() const{
	return m_isOpen;
}

inline void Updateable::perform(){
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_beforeCbk)
		m_beforeCbk->update();

	{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	update();
	}

	if(m_afterCbk)
		m_afterCbk->update();
}

}
