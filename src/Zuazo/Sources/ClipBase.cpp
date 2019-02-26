#include "ClipBase.h"

using namespace Zuazo::Sources;


/*
 * CLIP BASE'S METHODS
 */


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

VideoClipBase::VideoClipBase() :
	m_nbFrames(0),
	m_frameInterval()
{
}

/*
 * VIDEO CLIP BASE'S METHODS
 */

void VideoClipBase::setInfo(const Timing::TimeInterval& duration, u_int64_t frames, const Utils::Resolution& res){
	ClipBase::setInfo(duration);
	m_nbFrames=frames;
	m_frameInterval=Utils::Rational(duration.count(), frames * Timing::TimeInterval::TIME_UNITS_PER_SECOND);
	m_resolution=res;
}

