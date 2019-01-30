#include "Updateable.h"

#include "Timing.h"
#include "Utils/Rational.h"

using namespace Zuazo;

Updateable::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;

	setRate(0);
}

Updateable::Updateable(const Utils::Rational& rate){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;

	setRate(rate);
}

Updateable::Updateable(const Updateable& other){
	setInterval(other.m_updateInterval);
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
}

Updateable::~Updateable(){
	Timing::deleteTiming(this);
}

void Updateable::setBeforeUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_cbkMutex);
	m_beforeCbk=cbk;
}
void Updateable::setAfterUpdateCallback(Callback * cbk){
	std::lock_guard<std::mutex> lock(m_cbkMutex);
	m_afterCbk=cbk;
}

void Updateable::setInterval(const Utils::Rational& interval){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_updateInterval=interval;

	if(m_isOpen){
		Timing::TimeUnit tuInterval(m_updateInterval);
		Timing::modifyTiming(this, tuInterval);
	}
}

void Updateable::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

void Updateable::open(){
	if(!m_isOpen){
		m_isOpen=true;
		Timing::addTiming(this, Timing::TimeUnit(m_updateInterval));
	}
}

void Updateable::close(){
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_isOpen){
		Timing::deleteTiming(this);
		m_isOpen=false;
	}
}

void Updateable::perform(){
	{
		std::lock_guard<std::mutex> lock(m_cbkMutex);
		if(m_beforeCbk)
			m_beforeCbk->update();
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		update();
	}

	{
		std::lock_guard<std::mutex> lock(m_cbkMutex);
		if(m_afterCbk)
			m_afterCbk->update();
	}
}
