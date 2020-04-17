#include "Scheduler.h"

#include <map>
#include <algorithm>
#include <numeric>

namespace Zuazo::Timing {

void Scheduler::setEpoch(TimePoint tp) {
	m_epoch = tp;
}

TimePoint Scheduler::getEpoch() const {
	return m_epoch;
}


void Scheduler::gotoTime(TimePoint tp) {
		//Update time related variables
		m_deltaT = tp - m_currTime;
		m_currTime = tp;

		//Clear the update set to start over
		m_updates.clear();

		//Insert the regular updates
		m_updates.insert(
			m_updates.cend(), 
			m_regularEvents.cbegin(), 
			m_regularEvents.cend()
		);

		//Insert the periodic updates
		for(const auto& period : m_periodicEvents){
			if((m_currTime - m_epoch) % period.first == Duration::zero()){
				//This period needs to be updated
				m_updates.insert(
					m_updates.cend(),
					period.second.cbegin(), 
					period.second.cend()
				);
			}
		}

		//Sort the events from higher priorities to lower ones
		std::sort(
			m_updates.begin(), 
			m_updates.end(), 
			[](const auto& a, const auto& b) -> bool {
				return std::get<Priority>(a) > std::get<Priority>(b);
			}
		);

		//Update all
		for(const auto& event : m_updates){
			std::get<EventBaseRef>(event).get().update();
		}
}

TimePoint Scheduler::getTime() const {
	return m_currTime;
}

Duration Scheduler::getDeltaT() const {
	return m_deltaT;
}


Duration Scheduler::getTimeForNextEvent() const {
	//Calculate the minumum remaining time
	auto result = Duration::max();

	for(const auto& period : m_periodicEvents) {
		const auto nextUpdate = ((m_currTime - m_epoch) / period.first + 1) * period.first + m_epoch;
		const auto remaining = nextUpdate - m_currTime;

		if(remaining < result){
			result = remaining;
		}
	}

	return result;
}

void Scheduler::addRegularEvent(const EventBase& event, Priority prior) {
	m_regularEvents.emplace_back(event, prior);
}

void Scheduler::removeRegularEvent(const EventBase& event) {
	auto end = std::remove_if(
		m_regularEvents.begin(), 
		m_regularEvents.end(), 
		[&] (const auto& e) -> bool {
			return &std::get<EventBaseRef>(e).get() == &event; //Compare event's pointers
		}
	);
	m_regularEvents.erase(end, m_regularEvents.end());
}


void Scheduler::addPeriodicEvent(const EventBase& event, Priority prior, Duration period) {
	auto ite = m_periodicEvents.find(period);

	if(ite == m_periodicEvents.cend()){
		//Period does not exist. Create it.

		m_periodicEvents.emplace(	period, 
									EventSet{Event(event, prior)} );			
	} else {
		ite->second.emplace_back(event, prior);
	}
}

void Scheduler::removePeriodicEvent(const EventBase& event) {
	auto ite = m_periodicEvents.begin();

	while(ite != m_periodicEvents.end()) {
		//Erase it
		auto end = std::remove_if(
			ite->second.begin(), 
			ite->second.end(), 
			[&] (const auto& e) -> bool {
				return &std::get<EventBaseRef>(e).get() == &event; //Compare event's pointers
			}
		);
		ite->second.erase(end, ite->second.end());

		if(ite->second.size() == 0){
			//No more events of this period
			ite = m_periodicEvents.erase(ite);
		} else {
			++ite; //Advance
		}
	}
}

}