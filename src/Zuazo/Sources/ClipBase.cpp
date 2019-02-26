#include "ClipBase.h"

using namespace Zuazo::Sources;

ClipBase::ClipBase() :
	m_duration(Timing::TimeInterval::zero())
{
	m_hasOverflow=false;
}

ClipBase::ClipBase(const ClipBase& other) :
	m_duration(other.m_duration)
{
	m_hasOverflow=false;
}

void ClipBase::gotoTime(const Timing::TimeInterval& tp){
	if(m_currTime != tp){
		m_currTime=tp;

		if(m_currTime >=  m_duration){
			//New time is longer than the duration
			m_hasOverflow=true;
			m_currTime %= m_duration;

		}else if(m_currTime < Timing::TimeInterval::zero()){
			//New time is negative
			m_hasOverflow=true;

			while(m_currTime < Timing::TimeInterval::zero()){
				m_currTime += m_duration;
			}
		}

		perform();
	}
}

void ClipBase::setInfo(const Timing::TimeInterval& duration){
	m_duration=duration;
	m_currTime%=m_duration;
}
