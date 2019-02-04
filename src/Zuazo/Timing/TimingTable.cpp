#include "TimingTable.h"

#include <chrono>
#include <utility>

#include "PeriodicUpdate.h"
#include "RegularUpdate.h"


using namespace Zuazo::Timing;

/*
 * Timing modifiers
 */

void TimingTable::addTiming(Timing::PeriodicUpdate<UpdateOrder::FIRST>* event){
	TimeUnit interval=event->getInterval();

	if(interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_periodicTimings[interval].updates.first.insert(event);
	}
}

void TimingTable::addTiming(Timing::PeriodicUpdate<UpdateOrder::LAST>* event){
	TimeUnit interval=event->getInterval();

	if(interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_periodicTimings[interval].updates.last.insert(event);
	}
}

void TimingTable::addTiming(Timing::RegularUpdate<UpdateOrder::FIRST>* event){
	m_regularTimings.first.insert(event);
}

void TimingTable::addTiming(Timing::RegularUpdate<UpdateOrder::LAST>* event){
	m_regularTimings.last.insert(event);
}

void TimingTable::deleteTiming(Timing::PeriodicUpdate<UpdateOrder::FIRST>* event){
	for(auto element : m_periodicTimings)
		element.second.updates.first.erase(event);
	delUnusedInterval();
}

void TimingTable::deleteTiming(Timing::PeriodicUpdate<UpdateOrder::LAST>* event){
	for(auto element : m_periodicTimings)
		element.second.updates.last.erase(event);
	delUnusedInterval();
}

void TimingTable::deleteTiming(Timing::RegularUpdate<UpdateOrder::FIRST>* event){
	m_regularTimings.first.erase(event);
}

void TimingTable::deleteTiming(Timing::RegularUpdate<UpdateOrder::LAST>* event){
	m_regularTimings.last.erase(event);
}


TimingTable::PendingUpdates	TimingTable::getPendingUpdates(){
	PendingUpdates pend={
			TimeUnit::max(),
			m_regularTimings,
			PeriodicUpdates()
	};

	for(auto& timing : m_periodicTimings){
		const TimeUnit& interval=timing.first;
		UpdateInterval& periodicUpdates=timing.second;

		if(periodicUpdates.timeSinceLastUpdate >= interval){
			//This interval needs to be updated
			pend.periodicUpdates.first.insert(
					periodicUpdates.updates.first.begin(),
					periodicUpdates.updates.first.end());

			pend.periodicUpdates.last.insert(
					periodicUpdates.updates.last.begin(),
					periodicUpdates.updates.last.end());

			//Update the timing information
			periodicUpdates.timeSinceLastUpdate%=interval;
		}

		const TimeUnit timeForNextUpdate=interval - periodicUpdates.timeSinceLastUpdate;
		if(timeForNextUpdate < pend.timeForNextUpdate){
			//This interval's update is the soonest one
			pend.timeForNextUpdate=timeForNextUpdate;
		}
	}
	return pend;
}

/*
 * Private methods
 */

void TimingTable::addInterval(const TimeUnit& interval){
	//Add the new timing to the table
	if(m_periodicTimings.find(interval)==m_periodicTimings.end()){
		//Interval did not exist. Add it
		m_periodicTimings.emplace(interval, UpdateInterval());

		//Re-start all updates to synchronize them when possible
		for(auto& timing : m_periodicTimings)
			timing.second.timeSinceLastUpdate=TimeUnit(timing.first);
	}
}

void TimingTable::delUnusedInterval(){
	for(auto ite=m_periodicTimings.begin(); ite!=m_periodicTimings.begin();){
		if(ite->second.updates.first.size()==0 && ite->second.updates.last.size()==0)
			ite=m_periodicTimings.erase(ite);
		else
			ite++;
	}
}
