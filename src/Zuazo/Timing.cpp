#include "Timing.h"

#include <bits/stdint-intn.h>
#include <mutex>
#include <utility>

#include "Updateable.h"

using namespace Zuazo;

/*
 * Static variables
 */

std::map<Timing::TimeUnit, Timing::UpdateInterval>	Timing::s_timings;
std::thread Timing::s_thread;
bool Timing::s_exit;
std::mutex Timing::s_mutex;
std::condition_variable	Timing::s_cond;
Timing::TimePoint	Timing::s_currTime;

const int64_t Timing::TimeUnit::TIME_UNITS_PER_SECOND=Timing::TimeUnit( TIME_NAMESPACE::seconds(1) ).count();

/*
 * init/end functions
 */

int Timing::init(){
	s_timings=std::map<Timing::TimeUnit, Timing::UpdateInterval>();
	s_exit=false;
	s_thread=std::thread(updateThreadFunc);
	return 0;
}

int Timing::end(){
	s_exit=true;
	s_cond.notify_all();
	s_thread.join();
	return 0;
}

/*
 * Timing methods
 */

void Timing::addTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval){
	std::lock_guard<std::mutex> lock(s_mutex);

	if(event && interval.count()>0){
		addInterval(interval);
		//Insert the new element
		s_timings[interval].inputs.insert(event);
	}
}

void Timing::addTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval){
	std::lock_guard<std::mutex> lock(s_mutex);

	if(event && interval.count()>0){
		addInterval(interval);
		//Insert the new element
		s_timings[interval].outputs.insert(event);
	}
}

void Timing::deleteTiming(Periodic<UpdatePriority::INPUT>* event){
	std::unique_lock<std::mutex> lock(s_mutex);

	for(auto element : s_timings)
		element.second.inputs.erase(event);
	delUnusedInterval();
}

void Timing::deleteTiming(Periodic<UpdatePriority::CONSUMER>* event){
	std::unique_lock<std::mutex> lock(s_mutex);

	for(auto element : s_timings)
		element.second.outputs.erase(event);
	delUnusedInterval();
}

void Timing::modifyTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval){
	deleteTiming(event);
	addTiming(event, interval);
}

void Timing::modifyTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval){
	deleteTiming(event);
	addTiming(event, interval);
}

void Timing::addInterval(const TimeUnit& interval){
	//Add the new timing to the table
	if(s_timings.find(interval)==s_timings.end()){
		//Interval did not exist. Add it
		s_timings.emplace(std::pair<TimeUnit, UpdateInterval>(interval, UpdateInterval(interval)));

		//Re-start all updates to synchronize them when possible
		for(auto& timing : s_timings)
			timing.second.timeSinceLastUpdate=TimeUnit(timing.first);

		//Force update
		s_cond.notify_all();
	}
}

void Timing::delUnusedInterval(){
	for(auto ite=s_timings.begin(); ite!=s_timings.begin();){
		if(ite->second.inputs.size()==0 && ite->second.outputs.size()==0)
			ite=s_timings.erase(ite);
		else
			ite++;
	}
}

void Timing::updateThreadFunc(){
	std::unique_lock<std::mutex> lock(s_mutex);

	while(!s_exit){
		if(s_timings.size()){
			Chronometer chrono;
			chrono.start();

			TimeUnit timeForNextUpdate=TimeUnit::max();

			//Update all the pending events
			for(auto& timing : s_timings){
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
				for(auto& timing : s_timings){
					timing.second.timeSinceLastUpdate+=timeForNextUpdate;
				}

				//Wait until next update
				s_cond.wait_for(lock, timeForNextUpdate-elapsed);

				//Update current time
				s_currTime+=timeForNextUpdate;
			}else{
				// Increment timing values for each interval
				for(auto& timing : s_timings)
					timing.second.timeSinceLastUpdate+=elapsed;

				//Update current time
				s_currTime+=elapsed;
			}
		}else
			s_cond.wait(lock);
	}
}
