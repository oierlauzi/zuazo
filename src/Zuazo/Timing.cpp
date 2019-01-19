#include "Timing.h"

#include "Utils/Rational.h"
#include <type_traits>
#include <utility>

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
 * Timing::Updateable methods
 */

Updateable::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=true;
}

Updateable::Updateable(const Rational& rate) : Updateable(){
	setRate(rate);
}

Updateable::Updateable(const Updateable& other){
	setInterval(other.m_updateInterval);
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
}

Updateable::~Updateable(){
	setInterval(0);
}

void Updateable::setBeforeUpdateCallback(Callback * cbk){
	m_beforeCbk=cbk;
}
void Updateable::setAfterUpdateCallback(Callback * cbk){
	m_afterCbk=cbk;
}

void Updateable::setInterval(const Rational& interval){
	if(m_isOpen){
		m_updateInterval=interval;
		Timing::TimeUnit cInterval(m_updateInterval);
		Timing::addTiming(this, cInterval);
	}
}

void Updateable::setRate(const Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

/*
 * Timing methods
 */

void Timing::addTiming(Updateable* event, const TimeUnit& interval){
	std::unique_lock<std::mutex> lock(s_mutex);

	if(event && (interval.count() > 0)){
		if(interval.count()){
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

void Timing::modifyTiming(Updateable* event, const TimeUnit& interval){
	deleteTiming(event);
	addTiming(event, interval);
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

				while(updateData.timeSinceLastUpdate >= interval){
					//Members of this update interval need to be updated at least once
					for(Updateable * element : updateData.updateables){
						//Update all elements, indicating the timestamp and elapsed time
						element->perform();
					}

					//Update timing information
					updateData.timeSinceLastUpdate-=interval;
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
				//s_currTime+=timeForNextUpdate;
			}else{
				// Increment timing values for each interval
				for(auto& timing : s_timings)
					timing.second.timeSinceLastUpdate+=elapsed;

				//Update current time
				//s_currTime+=elapsed;
			}
		}else
			s_cond.wait(lock);
	}
}
