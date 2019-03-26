#pragma once

#include "../Utils/TimeInterval.h"
#include "../Utils/TimePoint.h"

namespace Zuazo::Misc{

class Chronometer{
public:
	Chronometer()=default;
	Chronometer(const Chronometer& other)=default;
	~Chronometer()=default;

	void								start();
	void								end();

	const Utils::TimeInterval&			getElapsed() const;
	const Utils::TimePoint&				getStart() const;
	const Utils::TimePoint&				getEnd() const;

private:
	Utils::TimePoint					m_start;
	Utils::TimePoint					m_end;
	Utils::TimeInterval					m_elapsed;
};


/*
 * METHOD DEFINITIONS
 */

inline void Chronometer::start(){
	m_start=Utils::TimePoint::now();
}

inline void Chronometer::end(){
	m_end=Utils::TimePoint::now();
	m_elapsed=m_end - m_start;
}

inline const Utils::TimeInterval& Chronometer::getElapsed() const{
	return m_elapsed;
}

inline const Utils::TimePoint& Chronometer::getStart() const{
	return m_start;
}

inline const Utils::TimePoint& Chronometer::getEnd() const{
	return m_end;
}

}
