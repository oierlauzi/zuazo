#pragma once

#include <sys/types.h>
#include <chrono>
#include <memory>

#include "../Utils/Rational.h"
#include "../Utils/TimeInterval.h"
#include "Timings.h"
#include "UpdateableBase.h"

namespace Zuazo{
extern std::unique_ptr<Timing::Timings> timings;
}

namespace Zuazo::Timing{

template <u_int32_t order>
class PeriodicUpdate :
		public virtual UpdateableBase
{
public:
	PeriodicUpdate()=default;
	PeriodicUpdate(const Utils::TimeInterval& interval);
	PeriodicUpdate(const Utils::Rational& rate);
	PeriodicUpdate(const PeriodicUpdate& other)=default;
	virtual ~PeriodicUpdate();

	virtual void 						setInterval(const Utils::TimeInterval& interval);
	virtual void						setRate(const Utils::Rational& rate);

	Utils::TimeInterval					getInterval() const;
	Utils::Rational						getRate() const;

	virtual void 						enable() override;
	virtual void 						disable() override;
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
inline PeriodicUpdate<order>::~PeriodicUpdate(){
	disable();
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::setInterval(const Utils::TimeInterval& interval){
	m_updateInterval=interval;

	timings->modifyTiming(this);
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::setRate(const Utils::Rational& rate){
	if(rate)
		PeriodicUpdate<order>::setInterval(Utils::TimeInterval(1/rate));
	else
		PeriodicUpdate<order>::setInterval(Utils::TimeInterval::zero());
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
inline void PeriodicUpdate<order>::enable(){
	timings->addTiming(this);
}

template <u_int32_t order>
inline void PeriodicUpdate<order>::disable(){
	if(timings)
		timings->deleteTiming(this);
}


}
