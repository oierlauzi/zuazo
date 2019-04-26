#include <NonLinearUpdate.h>

using namespace Zuazo;

std::map<UpdateableBase::UpdatePriority, Zuazo::NonLinearUpdate::Player> Zuazo::NonLinearUpdate::s_players;

NonLinearUpdate::NonLinearUpdate(UpdateableBase::UpdatePriority prior) :
        UpdateableBase(prior),
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}

NonLinearUpdate::NonLinearUpdate(UpdateableBase::UpdatePriority prior, const Timing::Duration& duration) :
        UpdateableBase(prior),
		m_duration(duration),
		m_state(States::Playing),
		m_repeat(true),
		m_playSpeed(1.0),
		m_playRev(false)
{
}

void NonLinearUpdate::gotoTime(const Timing::TimePoint& timepoint){
    Timing::TimePoint tp(timepoint);

	if(tp >=  Timing::TimePoint(m_duration)){
		//New time is longer than the duration
		if(m_repeat){
			tp = Timing::TimePoint(tp.time_since_epoch() % m_duration);
		}else{
			m_state=States::Paused; //We got to the end and repeat is not enabled. Pause
			tp = Timing::TimePoint(m_duration - Timing::Duration(1)); //Go to the end
		}
	}else if(tp < Timing::TimePoint()){
		//New time is negative
		if(m_repeat){
			while(tp < Timing::TimePoint()){
				tp += m_duration;
			}
		}else{
			m_state=States::Paused; //We got to the begining and repeat is not enabled. Pause
			tp = Timing::TimePoint(); //Go to the begining
		}
	}

	if(tp != m_currTime){
		//An update is needed
		m_currTime=tp;
		update();
	}
}

void NonLinearUpdate::enable(){
    //Try to find the update priority
    auto ite=s_players.find(m_updatePriority);
    if(ite == s_players.end()){
		auto res=s_players.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(m_updatePriority),
            std::forward_as_tuple(m_updatePriority)
        );

		ite=res.first;
    }

	//Iterator should be valid at this point
	ite->second.nonLinearUpdates.insert(this);
}

void NonLinearUpdate::disable(){
    //Try to find the update priority
    auto ite=s_players.find(m_updatePriority);
    if(ite != s_players.end()){
        ite->second.nonLinearUpdates.erase(this);

        //I was the last one in this update priority
        if(ite->second.nonLinearUpdates.empty()){
            s_players.erase(ite);
        }
    }
}


/*
 * PLAYER'S METHOD DEFINITIONS
 */

NonLinearUpdate::Player::Player(UpdatePriority prior) :
	RegularUpdate(prior)
{
	RegularUpdate::enable();
}

void NonLinearUpdate::Player::update() const{
	const Timing::Duration& elapsed=Timing::getTiming()->getElapsed();

	for(auto& nonlinear : nonLinearUpdates)
		nonlinear->advance(elapsed);
}