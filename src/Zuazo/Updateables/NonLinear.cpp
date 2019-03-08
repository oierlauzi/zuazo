#include "NonLinear.h"

#include "../Timing.h"
#include "../Zuazo.h"

using namespace Zuazo::Updateables;

NonLinear::NonLinear() :
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}


NonLinear::NonLinear(const Utils::TimeInterval& duration) :
		m_duration(duration),
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}

void NonLinear::open(){

}

void NonLinear::close(){
	m_lastUpdate=Utils::TimePoint();
}

void NonLinear::update() const{
	if(m_state==States::Playing && !m_hasEnded){
		Utils::TimeInterval elapsed;

		if(m_lastUpdate){
			elapsed=timings->now() - m_lastUpdate;
		}

		if(m_playRev){
			_gotoTime(m_currTime - elapsed * m_playSpeed);
		}else{
			_gotoTime(m_currTime + elapsed * m_playSpeed);
		}
	}

	m_lastUpdate=timings->now();
}


void NonLinear::_gotoTime(Utils::TimeInterval tp) const{
	if(tp >=  m_duration){
		//New time is longer than the duration
		if(m_repeat){
			m_hasEnded = false;
			tp %= m_duration;
		}else{
			m_hasEnded = true;
			tp = m_duration - Utils::TimeInterval((u_int64_t)1); //Go to the end
		}
	}else if(tp < Utils::TimeInterval::zero()){
		//New time is negative
		if(m_repeat){
			m_hasEnded = false;

			while(tp < Utils::TimeInterval::zero()){
				tp += m_duration;
			}
		}else{
			m_hasEnded = true;
			tp = Utils::TimeInterval::zero(); //Go to the begining
		}
	}


	if(tp != m_currTime){
		m_currTime=tp;
		nonLinearUpdate();
	}
}
