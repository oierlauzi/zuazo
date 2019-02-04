#include "TimingTable.h"

#include <chrono>
#include <cstdio>
#include <utility>

#include "Chronometer.h"
#include "PeriodicUpdate.h"
#include "RegularUpdate.h"

using namespace Zuazo::Timing;

/*
 * Constructor and destructor
 */


TimingTable::TimingTable(){
	m_exit=false;
	m_currTime=TimePoint::now();
	m_thread=std::thread(&TimingTable::updateThreadFunc, this);
}

TimingTable::~TimingTable(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}

/*
 * Timing methods
 */

void TimingTable::addTiming(Timing::PeriodicUpdate<UpdateOrder::FIRST>* event){
	TimeUnit interval=event->getInterval();

	std::lock_guard<std::mutex> lock(m_mutex);

	if(interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_periodicTimings[interval].first.insert(event);
	}
}

void TimingTable::addTiming(Timing::PeriodicUpdate<UpdateOrder::LAST>* event){
	TimeUnit interval=event->getInterval();

	std::lock_guard<std::mutex> lock(m_mutex);

	if(interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_periodicTimings[interval].last.insert(event);
	}
}

void TimingTable::addTiming(Timing::RegularUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_regularTimings.first.insert(event);
}

void TimingTable::addTiming(Timing::RegularUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_regularTimings.last.insert(event);
}

void TimingTable::deleteTiming(Timing::PeriodicUpdate<UpdateOrder::FIRST>* event){
	std::unique_lock<std::mutex> lock(m_mutex);

	for(auto element : m_periodicTimings)
		element.second.first.erase(event);
	delUnusedInterval();
}

void TimingTable::deleteTiming(Timing::PeriodicUpdate<UpdateOrder::LAST>* event){
	std::unique_lock<std::mutex> lock(m_mutex);

	for(auto element : m_periodicTimings)
		element.second.last.erase(event);
	delUnusedInterval();
}

void TimingTable::deleteTiming(Timing::RegularUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_regularTimings.first.erase(event);
}

void TimingTable::deleteTiming(Timing::RegularUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_regularTimings.last.erase(event);
}

void TimingTable::modifyTiming(Timing::PeriodicUpdate<UpdateOrder::FIRST>* event){
	deleteTiming(event);
	addTiming(event);
}

void TimingTable::modifyTiming(Timing::PeriodicUpdate<UpdateOrder::LAST>* event){
	deleteTiming(event);
	addTiming(event);
}

void TimingTable::addInterval(const TimeUnit& interval){
	//Add the new timing to the table
	if(m_periodicTimings.find(interval)==m_periodicTimings.end()){
		//Interval did not exist. Add it
		m_periodicTimings.emplace(interval, UpdateInterval());

		//Re-start all updates to synchronize them when possible
		for(auto& timing : m_periodicTimings)
			timing.second.timeSinceLastUpdate=TimeUnit(timing.first);

		//Force update
		m_cond.notify_all();
	}
}

void TimingTable::delUnusedInterval(){
	for(auto ite=m_periodicTimings.begin(); ite!=m_periodicTimings.begin();){
		if(ite->second.first.size()==0 && ite->second.last.size()==0)
			ite=m_periodicTimings.erase(ite);
		else
			ite++;
	}
}

void TimingTable::updateThreadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){
		if(m_periodicTimings.size()){

#ifdef ENABLE_LOAD_DEBUG
			Chronometer chrono;
			chrono.start();
#endif
			//Get all the intervals that will need an update and schedule the next update
			std::set<UpdateInterval*>  intervalsToUpdate;
			TimeUnit soonestIntervalUpdate=TimeUnit::max();

			for(auto& timing : m_periodicTimings){
				const TimeUnit& interval=timing.first;
				UpdateInterval& periodicEvents=timing.second;

				if(periodicEvents.timeSinceLastUpdate >= interval){
					//This interval needs to be updated
					intervalsToUpdate.insert(&periodicEvents);
					//Update the timing information
					periodicEvents.timeSinceLastUpdate%=interval;
				}

				const TimeUnit timeForNextUpdate=interval - periodicEvents.timeSinceLastUpdate;
				if(timeForNextUpdate < soonestIntervalUpdate){
					//This interval's update is the soonest one
					soonestIntervalUpdate=timeForNextUpdate;
				}
			}


			//Do all regular updates with first priority
			for(auto& regularEvent : m_regularTimings.first){
				regularEvent->perform();
			}

			//Do all peridodic updates with first priority
			for(auto& interval : intervalsToUpdate){
				for(auto& periodicEvent : interval->first){
					periodicEvent->perform();
				}
			}

			//Do all regular updates with last priority
			for(auto& regularEvent : m_regularTimings.last){
				regularEvent->perform();
			}

			//Do all peridodic updates with last priority
			for(auto& interval : intervalsToUpdate){
				for(auto& periodicEvent : interval->last){
					periodicEvent->perform();
				}
			}

			// Increment timing values for each interval and for currTime
			m_currTime+=soonestIntervalUpdate;
			for(auto& timing : m_periodicTimings){
				timing.second.timeSinceLastUpdate+=soonestIntervalUpdate;
			}

#ifdef ENABLE_LOAD_DEBUG
			chrono.end();
			printf("Elapsed: %ldus\n", chrono.getElapsed().count());
#endif

			//Wait until next update
			m_cond.wait_until(lock, m_currTime);
		}else
			m_cond.wait(lock);
	}
}
