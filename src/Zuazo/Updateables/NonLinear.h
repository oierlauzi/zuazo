#pragma once

#include <chrono>
#include <mutex>

#include "../Utils/TimeInterval.h"
#include "../Utils/TimePoint.h"
#include "Updateable.h"

namespace Zuazo::Updateables{

class NonLinear :
	virtual Updateable
{
public:
	enum class States{
		Playing,
		Paused,
	};

	NonLinear();
	NonLinear(const Utils::TimeInterval& duration);
	NonLinear(const NonLinear& other)=default;
	virtual ~NonLinear()=default;

	virtual void 				open() override;
	virtual void 				close() override;

	void 						gotoTime(const Utils::TimeInterval& tp);
	void 						gotoProgress(double p);
	void 						addTime(const Utils::TimeInterval& time);
	void 						subsTime(const Utils::TimeInterval& time);
	void						nextUpdate();
	void 						prevUpdate();

	void 						setState(States state);
	States						getState() const;

	void 						setPlaySpeed(double speed);
	double 						getPlaySpeed() const;

	void 						setRepeat(bool rep);
	bool 						getRepeat() const;

	bool 						hasEnded() const;

	Utils::TimeInterval 		getCurrentTime() const;
	Utils::TimeInterval 		getDuration() const;
	double 						getProgress() const;
protected:
	virtual void				update() const override;
	virtual void				nonLinearUpdate() const{};
	void						setInfo(const Utils::TimeInterval& duration);

	using Updateable::m_updateMutex;
private:
	Utils::TimeInterval 		m_duration;

	mutable Utils::TimeInterval	m_currTime;
	mutable Utils::TimePoint	m_lastUpdate;

	States						m_state;
	bool						m_repeat;
	double						m_playSpeed;
	bool						m_playRev;

	mutable bool				m_hasEnded	=false;
	void						_gotoTime(Utils::TimeInterval tp) const;
};

/*
 * METHOD DEFINITIONS
 */


inline void NonLinear::gotoTime(const Utils::TimeInterval& tp){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(tp);
}


inline void NonLinear::gotoProgress(double p){
	gotoTime(Utils::TimeInterval(m_duration * p));
}


inline void NonLinear::addTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(m_currTime + time);
}


inline void NonLinear::subsTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(m_currTime - time);
}


inline void NonLinear::setState(States state){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_state=state;
}


inline typename NonLinear::States NonLinear::getState() const{
	return m_state;
}


inline void NonLinear::setPlaySpeed(double speed){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	if(speed >= 0){
		m_playSpeed=speed;
		m_playRev=false;
	}else{
		m_playSpeed=-speed;
		m_playRev=true;
	}
}


inline double NonLinear::getPlaySpeed() const{
	return m_playRev ? -m_playSpeed : m_playSpeed;
}


inline void NonLinear::setRepeat(bool rep){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_repeat=rep;

	if(rep)
		m_hasEnded=false;
}


inline bool NonLinear::getRepeat() const{
	return m_repeat;
}


inline bool NonLinear::hasEnded() const{
	return m_hasEnded;
}


inline Utils::TimeInterval NonLinear::getCurrentTime() const{
	return m_currTime;
}


inline Utils::TimeInterval NonLinear::getDuration() const{
	return m_duration;
}


inline double NonLinear::getProgress() const{
	return (double)m_currTime.count() / m_duration.count() ;
}

inline void NonLinear::setInfo(const Utils::TimeInterval& duration){
	m_duration=duration;
	m_currTime%=m_duration;
}

}
