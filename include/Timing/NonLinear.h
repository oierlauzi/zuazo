#pragma once

#include <sys/types.h>
#include <chrono>
#include <mutex>
#include <set>

#include "RegularUpdate.h"
#include "Timings.h"
#include "../Utils/TimeInterval.h"

namespace Zuazo::Timing{

template <u_int32_t order>
class NonLinear :
	public virtual UpdateableBase
{
public:
	enum class States{
		Playing,
		Paused,
	};

	NonLinear();
	NonLinear(const Utils::TimeInterval& duration);
	NonLinear(const NonLinear& other)=default;
	virtual ~NonLinear();

	void 						gotoTime(const Utils::TimeInterval& tp);
	void 						gotoProgress(double p);
	void 						addTime(const Utils::TimeInterval& time);
	void 						subsTime(const Utils::TimeInterval& time);
	void						advance(const Utils::TimeInterval& time);

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

	virtual void 				enable() override;
	virtual void 				disable() override;
protected:
	void						setInfo(const Utils::TimeInterval& duration);
private:
	class Player :
			public RegularUpdate<order>
	{
		friend NonLinear;
	public:
		Player()=default;
		Player(const Player& other)=delete;
		~Player()=default;

		void addUpdate(NonLinear& update);
		void delUpdate(NonLinear& update);

		virtual void update() const override;
	protected:
		using Timing::UpdateableBase::m_updateMutex;
	private:
		std::set<NonLinear*> 		m_updates;
	};

	Utils::TimeInterval 		m_duration;
	Utils::TimeInterval			m_currTime;

	States						m_state;
	bool						m_repeat;
	double						m_playSpeed;
	bool						m_playRev;

	mutable std::mutex			m_ctrlMutex;

	bool						m_hasEnded	=false;
	void						_gotoTime(Utils::TimeInterval tp);

	static Player				s_player;
};


template <u_int32_t order>
inline typename NonLinear<order>::Player NonLinear<order>::s_player;

/*
 * METHOD DEFINITIONS
 */

template <u_int32_t order>
inline NonLinear<order>::NonLinear() :
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}

template <u_int32_t order>
inline NonLinear<order>::NonLinear(const Utils::TimeInterval& duration) :
		m_duration(duration),
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}

template <u_int32_t order>
inline NonLinear<order>::~NonLinear()
{
	s_player.delUpdate(*this);
}

template <u_int32_t order>
inline void NonLinear<order>::gotoTime(const Utils::TimeInterval& tp){
	std::lock_guard<std::mutex> lock(s_player.m_updateMutex);
	_gotoTime(tp);
}

template <u_int32_t order>
inline void NonLinear<order>::gotoProgress(double p){
	gotoTime(Utils::TimeInterval(m_duration * p));
}

template <u_int32_t order>
inline void NonLinear<order>::addTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(s_player.m_updateMutex);
	_gotoTime(m_currTime + time);
}

template <u_int32_t order>
inline void NonLinear<order>::subsTime(const Utils::TimeInterval& time){
	std::lock_guard<std::mutex> lock(s_player.m_updateMutex);
	_gotoTime(m_currTime - time);
}

template <u_int32_t order>
inline void	NonLinear<order>::advance(const Utils::TimeInterval& time){
	if(m_state == States::Playing)
		_gotoTime(m_currTime + time * getPlaySpeed());
}


template <u_int32_t order>
inline void NonLinear<order>::setState(States state){
	std::lock_guard<std::mutex> lock(s_player.updateMutex);
	m_state=state;
}

template <u_int32_t order>
inline typename NonLinear<order>::States NonLinear<order>::getState() const{
	return m_state;
}

template <u_int32_t order>
inline void NonLinear<order>::setPlaySpeed(double speed){
	std::lock_guard<std::mutex> lock(s_player.updateMutex);
	if(speed >= 0){
		m_playSpeed=speed;
		m_playRev=false;
	}else{
		m_playSpeed=-speed;
		m_playRev=true;
	}
}

template <u_int32_t order>
inline double NonLinear<order>::getPlaySpeed() const{
	return m_playRev ? -m_playSpeed : m_playSpeed;
}

template <u_int32_t order>
inline void NonLinear<order>::setRepeat(bool rep){
	std::lock_guard<std::mutex> lock(s_player.updateMutex);
	m_repeat=rep;

	if(rep)
		m_hasEnded=false;
}

template <u_int32_t order>
inline bool NonLinear<order>::getRepeat() const{
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
inline void NonLinear<order>::setInfo(const Utils::TimeInterval& duration){
	m_duration=duration;
	m_currTime%=m_duration;
}

template <u_int32_t order>
inline void NonLinear<order>::enable(){
	s_player.addUpdate(*this);
}

template <u_int32_t order>
inline void NonLinear<order>::disable(){
	s_player.delUpdate(*this);
}

template <u_int32_t order>
inline void NonLinear<order>::_gotoTime(Utils::TimeInterval tp){
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
		std::lock_guard<std::mutex> lock(m_updateMutex);
		update();
	}
}

template <u_int32_t order>
inline void NonLinear<order>::Player::addUpdate(NonLinear<order>& update){
	if(m_updates.size()){
		std::lock_guard<std::mutex> lock(m_updateMutex);
		m_updates.insert(&update);
	}else{
		m_updates.insert(&update);
		RegularUpdate<order>::enable();
	}
}

template <u_int32_t order>
inline void NonLinear<order>::Player::delUpdate(NonLinear<order>& update){
	{
		std::lock_guard<std::mutex> lock(m_updateMutex);
		m_updates.erase(&update);
	}

	if(m_updates.size() == 0)
		RegularUpdate<order>::disable();
}

template <u_int32_t order>
inline void NonLinear<order>::Player::update() const {
	const Utils::TimeInterval& elapsed=timings->getElapsed();

	for(auto& nonlinear : m_updates)
		nonlinear->advance(elapsed);
}


}
