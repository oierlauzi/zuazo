#include "TimingTable.h"

#include <chrono>
#include <initializer_list>
#include <utility>

#include "Chronometer.h"

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

void TimingTable::addTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(event && interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_timings[interval].inputs.insert(event);
	}
}

void TimingTable::addTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(event && interval.count()>0){
		addInterval(interval);
		//Insert the new element
		m_timings[interval].outputs.insert(event);
	}
}

void TimingTable::deleteTiming(Periodic<UpdatePriority::INPUT>* event){
	std::unique_lock<std::mutex> lock(m_mutex);

	for(auto element : m_timings)
		element.second.inputs.erase(event);
	delUnusedInterval();
}

void TimingTable::deleteTiming(Periodic<UpdatePriority::CONSUMER>* event){
	std::unique_lock<std::mutex> lock(m_mutex);

	for(auto element : m_timings)
		element.second.outputs.erase(event);
	delUnusedInterval();
}

void TimingTable::modifyTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval){
	deleteTiming(event);
	addTiming(event, interval);
}

void TimingTable::modifyTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval){
	deleteTiming(event);
	addTiming(event, interval);
}

void TimingTable::addInterval(const TimeUnit& interval){
	//Add the new timing to the table
	if(m_timings.find(interval)==m_timings.end()){
		//Interval did not exist. Add it
		m_timings.emplace(interval, UpdateInterval());

		//Re-start all updates to synchronize them when possible
		for(auto& timing : m_timings)
			timing.second.timeSinceLastUpdate=TimeUnit(timing.first);

		//Force update
		m_cond.notify_all();
	}
}

void TimingTable::delUnusedInterval(){
	for(auto ite=m_timings.begin(); ite!=m_timings.begin();){
		if(ite->second.inputs.size()==0 && ite->second.outputs.size()==0)
			ite=m_timings.erase(ite);
		else
			ite++;
	}
}

void TimingTable::updateThreadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){
		if(m_timings.size()){
			Chronometer chrono;
			chrono.start();

			TimeUnit timeForNextUpdate=TimeUnit::max();

			//Update all the pending events
			for(auto& timing : m_timings){
				const TimeUnit& interval=timing.first;
				UpdateInterval& updateData=timing.second;

				if(updateData.timeSinceLastUpdate >= interval){
					//Members of this update interval need to be updated at least once
					for(Periodic<UpdatePriority::INPUT> * element : updateData.inputs){
						element->perform();
					}

					for(Periodic<UpdatePriority::CONSUMER> * element : updateData.outputs){
						element->perform();
					}

					//Update timing information
					updateData.timeSinceLastUpdate%=interval;
				}

				//Check if the next update for this interval is sooner than the previous one
				const TimeUnit timeForNextIntervalUpdate=interval - updateData.timeSinceLastUpdate;
				if(timeForNextIntervalUpdate < timeForNextUpdate){
					timeForNextUpdate=timeForNextIntervalUpdate;
				}
			}

			chrono.end();
			TimeUnit elapsed=chrono.getElapsed();

			if(elapsed < timeForNextUpdate){
				// Increment timing values for each interval
				for(auto& timing : m_timings){
					timing.second.timeSinceLastUpdate+=timeForNextUpdate;
				}

				//Update current time
				m_currTime+=timeForNextUpdate;


				//Wait until next update
				m_cond.wait_until(lock, m_currTime);
			}else{
				// Increment timing values for each interval
				for(auto& timing : m_timings)
					timing.second.timeSinceLastUpdate+=elapsed;

				//Update current time
				m_currTime+=elapsed;
			}

#ifdef ENABLE_LOAD_DEBUG
			printf("Elapsed: %ldus\n", elapsed.count());
#endif

		}else
			m_cond.wait(lock);
	}
}
