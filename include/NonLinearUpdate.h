#pragma once

#include "Timing.h"

#include <map>

namespace Zuazo{

class NonLinearUpdate : public virtual UpdateableBase {
public:
    enum class States{
        Playing,
        Paused,
    };

    NonLinearUpdate(UpdateableBase::UpdatePriority prior);
    NonLinearUpdate(UpdateableBase::UpdatePriority prior, const Timing::Duration& duration);
    NonLinearUpdate(const NonLinearUpdate& other)=default;
    virtual ~NonLinearUpdate();

    void 					        	gotoTime(const Timing::TimePoint& timepoint);
    void 					        	gotoTime(const Timing::Duration& timepoint);
    void 					        	gotoProgress(double p);
    void 					        	addTime(const Timing::Duration& time);
    void 					        	subsTime(const Timing::Duration& time);
    void						        advance(const Timing::Duration& time);

    void 						        setState(States state);
    States						        getState() const;

    void 						        setPlaySpeed(double speed);
    double 						        getPlaySpeed() const;

    void 						        setRepeat(bool rep);
    bool 						        getRepeat() const;

    const Timing::Duration& 		    getDuration() const;
    const Timing::TimePoint& 		    getCurrentTime() const;
    double 						        getProgress() const;

    virtual void 				        enable() override;
    virtual void 				        disable() override;
protected:
    void						        setData(const Timing::Duration& duration);
private:
    struct Player : public Timing::RegularUpdate{
        Player(UpdatePriority prior);
        Player(const Player& other)=delete;
        ~Player()=default;

        virtual void                        update() const override;
        std::set<NonLinearUpdate*> 	        nonLinearUpdates;
    };

    Timing::Duration 		            m_duration;
    Timing::TimePoint			        m_currTime;

    States						        m_state;
    bool						        m_repeat;
    double						        m_playSpeed;
    bool						        m_playRev;

    static std::map<UpdateableBase::UpdatePriority, Player> s_players;
};

/*
 * NON LINEAR UPDATE'S INLINE METHOD DEFINITIONS
 */

inline NonLinearUpdate::~NonLinearUpdate(){
    disable();
}

inline void NonLinearUpdate::gotoTime(const Timing::Duration& timepoint){
    gotoTime(Timing::TimePoint(timepoint));
}

inline void NonLinearUpdate::gotoProgress(double p){
	gotoTime(Timing::Duration(m_duration * p));
}

inline void NonLinearUpdate::addTime(const Timing::Duration& time){
	gotoTime(m_currTime + time);
}

inline void NonLinearUpdate::subsTime(const Timing::Duration& time){
	gotoTime(m_currTime - time);
}

inline void	NonLinearUpdate::advance(const Timing::Duration& time){
	if(m_state == States::Playing)
		gotoTime(m_currTime + Timing::Duration(time * getPlaySpeed()));
}


inline void NonLinearUpdate::setState(States state){
	m_state=state;
}

inline NonLinearUpdate::States NonLinearUpdate::getState() const{
	return m_state;
}

inline void NonLinearUpdate::setPlaySpeed(double speed){
    m_playRev=speed < 0;
    m_playSpeed=m_playRev ? -speed : speed;
}

inline double NonLinearUpdate::getPlaySpeed() const{
	return m_playRev ? -m_playSpeed : m_playSpeed;
}

inline void NonLinearUpdate::setRepeat(bool rep){
	m_repeat=rep;
}

inline bool NonLinearUpdate::getRepeat() const{
	return m_repeat;
}

inline const Timing::TimePoint& NonLinearUpdate::getCurrentTime() const{
	return m_currTime;
}

inline const Timing::Duration& NonLinearUpdate::getDuration() const{
	return m_duration;
}

inline double NonLinearUpdate::getProgress() const{
	return (double)m_currTime.time_since_epoch().count() / m_duration.count() ;
}

inline void NonLinearUpdate::setData(const Timing::Duration& duration){
	m_duration=duration;
	m_currTime=Timing::TimePoint();
}

}