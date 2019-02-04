#pragma once

#include <memory>

#include "../Updateable.h"
#include "../Utils/Rational.h"
#include "TimeUnit.h"
#include "Timing.h"
#include "UpdateOrder.h"

namespace Zuazo::Timing{

extern std::unique_ptr<Timings> timings;

template <UpdateOrder TPriority>
class PeriodicUpdate : public Updateable{
	friend Timings;
public:
	PeriodicUpdate();
	PeriodicUpdate(const Utils::Rational& rate);
	PeriodicUpdate(const PeriodicUpdate& other);
	virtual ~PeriodicUpdate();

	const Utils::Rational& 				getInterval() const;
	Utils::Rational						getRate() const;

	virtual void						open() override;
	virtual void						close() override;
protected:
	void 								setInterval(const Utils::Rational& interval);
	void								setRate(const Utils::Rational& rate);
private:
	Utils::Rational						m_updateInterval;
};

/*
 * METHOD DEFINITIONS
 */

template <UpdateOrder TPriority>
inline PeriodicUpdate<TPriority>::PeriodicUpdate() : Updateable(){

}

template <UpdateOrder TPriority>
inline PeriodicUpdate<TPriority>::PeriodicUpdate(const Utils::Rational& rate) : PeriodicUpdate(){
	setRate(rate);
}

template <UpdateOrder TPriority>
inline PeriodicUpdate<TPriority>::PeriodicUpdate(const PeriodicUpdate& other) : Updateable(other){
	setInterval(other.m_updateInterval);
}

template <UpdateOrder TPriority>
inline PeriodicUpdate<TPriority>::~PeriodicUpdate(){
	if(timings)
		timings->deleteTiming(this);
}



template <UpdateOrder TPriority>
inline const Utils::Rational& PeriodicUpdate<TPriority>::getInterval() const {
	return m_updateInterval;
}

template <UpdateOrder TPriority>
inline Utils::Rational PeriodicUpdate<TPriority>::getRate() const {
	return 1/m_updateInterval;
}


template <UpdateOrder TPriority>
inline void PeriodicUpdate<TPriority>::open(){
	Updateable::open();
	timings->addTiming(this);
}

template <UpdateOrder TPriority>
inline void PeriodicUpdate<TPriority>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}

template <UpdateOrder TPriority>
inline void PeriodicUpdate<TPriority>::setInterval(const Utils::Rational& interval){
	m_updateInterval=interval;

	if(isOpen()){
		timings->modifyTiming(this);
	}
}

template <UpdateOrder TPriority>
inline void PeriodicUpdate<TPriority>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

}
