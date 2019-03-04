#pragma once

#include <sys/types.h>
#include <chrono>
#include <mutex>

#include "../Utils/TimeInterval.h"
#include "PeriodicUpdate.h"

namespace Zuazo::Updateables{


template <u_int32_t order>
class NonLinear :
		public PeriodicUpdate<order>
{
public:
	enum class States{
		Playing,
		Paused,
	};

	NonLinear();
	NonLinear(const Utils::TimeInterval& duration, const Utils::TimeInterval& updateInterval);
	NonLinear(const NonLinear& other);
	virtual ~NonLinear()=default;

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

	using PeriodicUpdate<order>::open;
	using PeriodicUpdate<order>::close;
protected:
	virtual void				update() const override;
	virtual void				nonLinearUpdate() const{};
	void						setInfo(const Utils::TimeInterval& duration, const Utils::TimeInterval& updateInterval);

	using Updateable::m_updateMutex;
private:
	Utils::TimeInterval 		m_duration;
	Utils::TimeInterval 		m_updateInterval;

	mutable Utils::TimeInterval	m_currTime;

	States						m_state		=States::Playing;
	bool						m_repeat;
	double						m_playSpeed	=1.0;
	bool						m_playRev=false;

	mutable bool				m_hasEnded	=false;
	void						_gotoTime(Utils::TimeInterval tp) const;
};

/*
 * METHOD DEFINITIONS
 */

template <u_int32_t order>
inline NonLinear<order>::NonLinear(){
	m_repeat=true;
}

template <u_int32_t order>
inline NonLinear<order>::NonLinear(const Utils::TimeInterval& duration, const Utils::TimeInterval& updateInterval) :
	PeriodicUpdate<order>(updateInterval),
	m_duration(duration),
	m_updateInterval(updateInterval)
{
	m_repeat=true;
}


template <u_int32_t order>
inline NonLinear<order>::NonLinear(const NonLinear<order>& other) :
	PeriodicUpdate<order>(other),
	m_duration(other.m_duration),
	m_updateInterval(other.m_updateInterval)
{
	m_repeat=true;
	//TODO
}

template <u_int32_t order>
void NonLinear<order>::gotoTime(const Utils::TimeInterval& tp){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(tp);
}

template <u_int32_t order>
inline void NonLinear<order>::gotoProgress(double p){
	gotoTime(Utils::TimeInterval(m_duration * p));
}

template <u_int32_t order>
inline void NonLinear<order>::addTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(m_currTime + time);
}

template <u_int32_t order>
inline void NonLinear<order>::subsTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	_gotoTime(m_currTime - time);
}

template <u_int32_t order>
void NonLinear<order>::nextUpdate(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	addTime(m_updateInterval);
}

template <u_int32_t order>
void NonLinear<order>::prevUpdate(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	subsTime(m_updateInterval);
}

template <u_int32_t order>
void NonLinear<order>::setState(States state){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_state=state;
}

template <u_int32_t order>
typename NonLinear<order>::States NonLinear<order>::getState() const{
	return m_state;
}

template <u_int32_t order>
void NonLinear<order>::setPlaySpeed(double speed){
	if(speed >= 0){
		m_playSpeed=speed;
		m_playRev=false;
	}else{
		m_playSpeed=-speed;
		m_playRev=true;
	}

	PeriodicUpdate<order>::setInterval(m_updateInterval * speed);
}

template <u_int32_t order>
double NonLinear<order>::getPlaySpeed() const{
	return m_playRev ? -m_playSpeed : m_playSpeed;
}

template <u_int32_t order>
void NonLinear<order>::setRepeat(bool rep){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_repeat=rep;

	if(rep)
		m_hasEnded=false;
}

template <u_int32_t order>
bool NonLinear<order>::getRepeat() const{
	return m_repeat;
}

template <u_int32_t order>
inline bool NonLinear<order>::hasEnded() const{
	return m_hasEnded;
}

template <u_int32_t order>
inline Utils::TimeInterval NonLinear<order>::getCurrentTime() const{
	return m_currTime;
}

template <u_int32_t order>
inline Utils::TimeInterval NonLinear<order>::getDuration() const{
	return m_duration;
}

template <u_int32_t order>
inline double NonLinear<order>::getProgress() const{
	return (double)m_currTime.count() / m_duration.count() ;
}

template <u_int32_t order>
inline void	NonLinear<order>::update() const{
	if(m_state == States::Playing && !m_hasEnded){
		if(m_playRev){
			_gotoTime(m_currTime - m_updateInterval);
		}else{
			_gotoTime(m_currTime + m_updateInterval);		}
	}
}

template <u_int32_t order>
inline void	NonLinear<order>::_gotoTime(Utils::TimeInterval tp) const{
	if(tp >=  m_duration){
		//New time is longer than the duration
		if(m_repeat){
			m_hasEnded = false;
			tp %= m_duration;
		}else{
			m_hasEnded = true;
			tp = m_duration - m_updateInterval; //Go to the end
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

template <u_int32_t order>
void NonLinear<order>::setInfo(const Utils::TimeInterval& duration, const Utils::TimeInterval& updateInterval){
	m_duration=duration;
	m_updateInterval=updateInterval;

	PeriodicUpdate<order>::setInterval(m_updateInterval);
	m_currTime%=m_duration;
}

}
