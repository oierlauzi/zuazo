#pragma once

#include <bits/stdint-intn.h>
#include <chrono>

#include "../Timing/TimeInterval.h"
#include "SourceBase.h"

namespace Zuazo::Sources{

class ClipBase : virtual public SourceBase
{
public:
	ClipBase();
	ClipBase(const ClipBase& other);
	virtual ~ClipBase()=default;

	void gotoTime(const Timing::TimeInterval& tp);
	void gotoProgress(double p);
	void addTime(const Timing::TimeInterval& time);
	void subsTime(const Timing::TimeInterval& time);

	bool hasOverflow() const;
	void resetOverflow();

	Timing::TimeInterval getCurrentTime() const;
	Timing::TimeInterval getDuration() const;
	double getProgress() const;
protected:
	void setInfo(const Timing::TimeInterval& duration);
private:
	Timing::TimeInterval		m_currTime;
	Timing::TimeInterval 		m_duration;
	bool						m_hasOverflow;
};

/*
 * INLINE METHODS
 */

inline void ClipBase::gotoProgress(double p){
	gotoTime(Timing::TimeInterval(
			static_cast<int64_t>(m_duration.count() * p)
	));
}

inline void ClipBase::addTime(const Timing::TimeInterval& time){
	gotoTime(m_currTime + time);
}

inline void ClipBase::subsTime(const Timing::TimeInterval& time){
	gotoTime(m_currTime - time);
}

inline bool ClipBase::hasOverflow() const{
	return m_hasOverflow;
}

inline void ClipBase::resetOverflow(){
	m_hasOverflow=false;
}

inline Timing::TimeInterval ClipBase::getCurrentTime() const{
	return m_currTime;
}

inline Timing::TimeInterval ClipBase::getDuration() const{
	return m_duration;
}

inline double ClipBase::getProgress() const{
	return (double)m_currTime.count() / m_duration.count() ;
}
}
