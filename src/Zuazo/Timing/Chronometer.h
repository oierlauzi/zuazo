#pragma once

#include "TimePoint.h"

#define TIME_CLOCK steady_clock

namespace Zuazo::Timing{

class Chronometer{
public:
	Chronometer()=default;
	Chronometer(const Chronometer& other)=default;
	~Chronometer()=default;

	void								start();
	void								end();
	const TimeInterval&						getElapsed() const;
	const TimePoint&					getStart() const;
	const TimePoint&					getEnd() const;

private:
	TimePoint							m_start;
	TimePoint							m_end;
	TimeInterval							m_elapsed;
};


/*
 * METHOD DEFINITIONS
 */

inline void Timing::Chronometer::start(){
	m_start=TimePoint::now();
}

inline void Timing::Chronometer::end(){
	m_end=TimePoint::now();
	m_elapsed=m_end - m_start;
}

inline const TimeInterval&	 Timing::Chronometer::getElapsed() const{
	return m_elapsed;
}

inline const TimePoint& Timing::Chronometer::getStart() const{
	return m_start;
}

inline const TimePoint& Timing::Chronometer::getEnd() const{
	return m_end;
}

}
