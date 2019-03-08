#pragma once

#include <sys/types.h>
#include <chrono>
#include <memory>

#include "../Timing.h"
#include "../Utils/Rational.h"
#include "../Utils/TimeInterval.h"
#include "Updateable.h"

namespace Zuazo{
extern std::unique_ptr<Timing> timings;
}

namespace Zuazo::Updateables{

template <u_int32_t order>
class PeriodicUpdate :
		public virtual Updateable
{
public:
	PeriodicUpdate()=default;
	PeriodicUpdate(const Utils::TimeInterval& interval);
	PeriodicUpdate(const Utils::Rational& rate);
	PeriodicUpdate(const PeriodicUpdate& other);
	virtual ~PeriodicUpdate()=default;

	virtual void 						setInterval(const Utils::TimeInterval& interval);
	virtual void						setRate(const Utils::Rational& rate);

	Utils::TimeInterval					getInterval() const;
	Utils::Rational						getRate() const;

	virtual void						open() override;
	virtual void						close() override;
private:
	Utils::Rational						m_updateInterval;
};

/*
 * METHOD DEFINITIONS
 */

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate(const Utils::TimeInterval& interval) :
	m_updateInterval(interval)
{
}

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate(const Utils::Rational& rate) :
	m_updateInterval(1 / rate)
{
}

template <u_int32_t order>
inline PeriodicUpdate<order>::PeriodicUpdate(const PeriodicUpdate& other) :
	m_updateInterval(other.m_updateInterval)
{
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::setInterval(const Utils::TimeInterval& interval){
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
		setInterval(Utils::TimeInterval::zero());
}

template <u_int32_t order>
inline Utils::TimeInterval PeriodicUpdate<order>::getInterval() const {
	return m_updateInterval;
}

template <u_int32_t order>
inline Utils::Rational PeriodicUpdate<order>::getRate() const {
	return 1/m_updateInterval;
}


template <u_int32_t order>
inline void PeriodicUpdate<order>::open(){
	timings->addTiming(this);
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::close(){
	if(timings) //It might have been deleted
		timings->deleteTiming(this);
}

}
