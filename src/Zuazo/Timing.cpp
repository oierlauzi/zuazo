#include "Timing.h"

#include "Utils/Rational.h"
#include <type_traits>
#include <utility>

using namespace Zuazo;

/*
 * Static variables
 */

std::map<Timing::time_unit, Timing::UpdateInterval>	Timing::s_timings;
std::thread Timing::s_thread;
bool Timing::s_exit;
std::mutex Timing::s_mutex;
std::condition_variable	Timing::s_cond;

/*
 * init/end functions
 */

int Timing::init(){
	s_timings=std::map<Timing::time_unit, Timing::UpdateInterval>();
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
 * Timing::Updateable methods
 */

Timing::Updateable::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;
}

Timing::Updateable::Updateable(const Rational& rate) : Updateable(){
	setRate(rate);
}

Timing::Updateable::Updateable(const Updateable& other){
	setInterval(other.m_updateInterval);
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
}

Timing::Updateable::~Updateable(){
	setInterval(0);
}

void Timing::Updateable::setBeforeUpdateCallback(Callback * cbk){
	m_beforeCbk=cbk;
}
void Timing::Updateable::setAfterUpdateCallback(Callback * cbk){
	m_afterCbk=cbk;
}

void Timing::Updateable::setInterval(const Rational& interval){
	if(m_isOpen){
		m_updateInterval=interval;
		time_unit cInterval=toTu(m_updateInterval);
		Timing::addTiming(this, cInterval);
	}
}

void Timing::Updateable::setRate(const Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

/*
 * Timing methods
 */

void Timing::addTiming(Updateable* event, const time_unit& interval){
	std::unique_lock<std::mutex> lock(s_mutex);

	if(event && (interval.count() > 0)){
		if(interval.count()){
			//Add the new timing to the table
			if(s_timings.find(interval)==s_timings.end()){
				//Interval did not exist. Add it
				s_timings.emplace(std::pair<time_unit, UpdateInterval>(interval, UpdateInterval(interval)));

				//Re-start all updates to synchronize them when possible
				for(auto& timing : s_timings)
					timing.second.timeSinceLastUpdate=time_unit(timing.first);

				//Force update
				s_cond.notify_all();
			}
			s_timings[interval].updateables.insert(event);
		}
	}
}

void Timing::deleteTiming(Updateable* event){
	std::unique_lock<std::mutex> lock(s_mutex);

	for(auto ite=s_timings.begin(); ite!=s_timings.begin();){
		ite->second.updateables.erase(event);

		if(ite->second.updateables.size()==0)
			ite=s_timings.erase(ite);
		else
			ite++;
	}
}

void Timing::modifyTiming(Updateable* event, const time_unit& interval){
	deleteTiming(event);
	addTiming(event, interval);
}

void Timing::updateThreadFunc(){
	std::unique_lock<std::mutex> lock(s_mutex);

	while(!s_exit){
		if(s_timings.size()){
			Chronometer chrono;
			chrono.start();

			time_unit timeForNextUpdate=time_unit::max();

			//Update all the pending events
			for(auto& timing : s_timings){
				const time_unit& interval=timing.first;
				UpdateInterval& updateData=timing.second;

				while(updateData.timeSinceLastUpdate >= interval){
					//Members of this update interval need to be updated at least once
					for(Updateable * element : updateData.updateables){
						//Update all elements, indicating the timestamp and elapsed time
						element->perform(updateData.timeSinceLastUpdate, chrono.getStart());
					}

					//Update timing information
					updateData.timeSinceLastUpdate-=interval;
				}

				//Check if the next update for this interval is sooner than the previous one
				const time_unit timeForNextIntervalUpdate=interval - updateData.timeSinceLastUpdate;
				if(timeForNextIntervalUpdate < timeForNextUpdate){
					timeForNextUpdate=timeForNextIntervalUpdate;
				}
			}

			chrono.end();
			time_unit elapsed=chrono.getElapsed();

			if(elapsed < timeForNextUpdate){
				// Increment timing values for each interval
				for(auto& timing : s_timings){
					timing.second.timeSinceLastUpdate+=timeForNextUpdate;
				}

				//Wait until next update
				s_cond.wait_for(lock, timeForNextUpdate-elapsed);
			}else{
				// Increment timing values for each interval
				for(auto& timing : s_timings)
					timing.second.timeSinceLastUpdate+=elapsed;
			}
		}else
			s_cond.wait(lock);
	}
}
