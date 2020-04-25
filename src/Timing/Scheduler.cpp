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
		m_calls.clear();

		//Insert the regular updates
		m_calls.insert(
			m_calls.cend(), 
			m_regularCallbacks.cbegin(), 
			m_regularCallbacks.cend()
		);

		//Insert the periodic callbacks
		for(const auto& period : m_periodicCallbacks){
			if((m_currTime - m_epoch) % period.first == Duration::zero()){
				//This period needs to be updated
				m_calls.insert(
					m_calls.cend(),
					period.second.cbegin(), 
					period.second.cend()
				);
			}
		}

		//Sort the callbacks from higher priorities to lower ones
		std::stable_sort(
			m_calls.begin(), 
			m_calls.end(), 
			[](const auto& a, const auto& b) -> bool {
				return std::get<Priority>(a) > std::get<Priority>(b);
			}
		);

		//Call all
		for(const auto& c : m_calls){
			const Callback& cbk = std::get<CallbackRef>(c);
			if(cbk) {
				cbk();
			}
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

	for(const auto& period : m_periodicCallbacks) {
		const auto nextUpdate = ((m_currTime - m_epoch) / period.first + 1) * period.first + m_epoch;
		const auto remaining = nextUpdate - m_currTime;

		if(remaining < result){
			result = remaining;
		}
	}

	return result;
}

void Scheduler::addRegularCallback(const Callback& cbk, Priority prior) {
	m_regularCallbacks.emplace_back(cbk, prior);
}

void Scheduler::removeRegularCallback(const Callback& cbk) {
	auto end = std::remove_if(
		m_regularCallbacks.begin(), 
		m_regularCallbacks.end(), 
		[&] (const auto& e) -> bool {
			return &std::get<CallbackRef>(e).get() == &cbk; //Compare callback's pointers
		}
	);
	m_regularCallbacks.erase(end, m_regularCallbacks.end());
}


void Scheduler::addPeriodicCallback(const Callback& cbk, Priority prior, Duration period) {
	auto ite = m_periodicCallbacks.find(period);

	if(ite == m_periodicCallbacks.cend()){
		//Period does not exist. Create it.

		m_periodicCallbacks.emplace(period, 
									CallbackSet{CallbackSet::value_type(cbk, prior)} );			
	} else {
		ite->second.emplace_back(cbk, prior);
	}
}

void Scheduler::removePeriodicCallback(const Callback& cbk) {
	auto ite = m_periodicCallbacks.begin();

	while(ite != m_periodicCallbacks.end()) {
		//Erase it
		auto end = std::remove_if(
			ite->second.begin(), 
			ite->second.end(), 
			[&] (const auto& e) -> bool {
				return &std::get<CallbackRef>(e).get() == &cbk; //Compare callback's pointers
			}
		);
		ite->second.erase(end, ite->second.end());

		if(ite->second.size() == 0){
			//No more callbacks of this period
			ite = m_periodicCallbacks.erase(ite);
		} else {
			++ite; //Advance
		}
	}
}

}