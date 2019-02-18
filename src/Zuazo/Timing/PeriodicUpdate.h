#pragma once

#include <sys/types.h>
#include <memory>

#include "../Updateable.h"
#include "../Utils/Rational.h"
#include "Timing.h"

namespace Zuazo::Timing{

template <u_int32_t order>
class PeriodicUpdate :
		public virtual Updateable
{
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

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate() : Updateable(){

}

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate(const Utils::Rational& rate) : PeriodicUpdate(){
	setRate(rate);
}

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate(const PeriodicUpdate& other) : Updateable(other){
	setInterval(other.m_updateInterval);
}

template <u_int32_t order>
inline PeriodicUpdate<order>::~PeriodicUpdate(){
	if(timings)
		timings->deleteTiming(this);
}



template <u_int32_t order>
inline const Utils::Rational& PeriodicUpdate<order>::getInterval() const {
	return m_updateInterval;
}

template <u_int32_t order>
inline Utils::Rational PeriodicUpdate<order>::getRate() const {
	return 1/m_updateInterval;
}


template <u_int32_t order>
inline void PeriodicUpdate<order>::open(){
	Updateable::open();
	timings->addTiming(this);
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::setInterval(const Utils::Rational& interval){
	m_updateInterval=interval;

	if(isOpen()){
		timings->modifyTiming(this);
	}
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

}
