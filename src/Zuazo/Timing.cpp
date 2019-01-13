#include "Timing.h"

#include <utility>

#include "Utils/Rational.h"

using namespace Zuazo;

Timing::TimingTable	Timing::s_timings;

Timing::Timing() {
	//Does not get updated by the loop
}

Timing::Timing(const Rational& rate) {
	setRate(rate);
}

Timing::Timing(const Timing& other) : Timing(other.m_updateInterval){

}

Timing::~Timing() {
	s_timings.deleteTiming(this);
}



void Timing::setInterval(const Rational& interval) {
	m_updateInterval=interval;

	time_unit cInterval=toTimeUnit(m_updateInterval);
	if(cInterval.count() > 0)
		s_timings.addTiming(this, cInterval);
}

void Timing::setRate(const Rational& rate){
	setInterval(1/rate);
}


/*
 *	TIMING TABLE
 */

Timing::TimingTable::TimingTable(){
	m_exit=false;
	m_thread=std::thread(&Timing::TimingTable::updateThreadFunc, this);
}

Timing::TimingTable::~TimingTable(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}





void Timing::TimingTable::addTiming(Timing* element, time_unit interval){
	std::unique_lock<std::mutex> lock(m_mutex);
	if(element){
		if(interval.count()){
			//Add the new timing to the table
			if(m_table.find(interval)==m_table.end()){
				//Interval did not exist. Add it
				m_table.emplace(std::make_pair(interval, TimingData()));

				//Re-start all updates to synchronize them when possible
				for(auto& timing : m_table)
					timing.second.timeSinceLastUpdate=time_unit(0);
			}
			m_table[interval].elements.insert(element);

			//Force update
			m_cond.notify_all();
		}
	}
}

void Timing::TimingTable::modifyTiming(Timing* element, time_unit interval){
	deleteTiming(element);
	addTiming(element, interval);
}

void Timing::TimingTable::deleteTiming(Timing* element){
	std::unique_lock<std::mutex> lock(m_mutex);
	for(auto ite=m_table.begin(); ite!=m_table.begin();){
		ite->second.elements.erase(element);

		if(ite->second.elements.size()==0)
			ite=m_table.erase(ite);
		else
			ite++;
	}
}

void Timing::TimingTable::updateThreadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){
		if(m_table.size()){
			std::chrono::time_point<std::chrono::steady_clock> start, end;
			start=std::chrono::steady_clock::now();

			time_unit timeForNextUpdate=time_unit::max();

			//Update all the pending updates
			for(auto& timing : m_table){
				const time_unit& interval=timing.first;
				TimingData& updateData=timing.second;

				if(updateData.timeSinceLastUpdate >= interval){
					//Members of this update interval need to be updated
					for(Timing * element : updateData.elements)
						element->update(updateData.timeSinceLastUpdate);

					//Update timing information
					updateData.timeSinceLastUpdate-=interval;
				}

				//Check if the next update for this interval is sooner than the previous one
				const time_unit timeForNextIntervalUpdate=interval - updateData.timeSinceLastUpdate;
				if(timeForNextIntervalUpdate < timeForNextUpdate){
					timeForNextUpdate=timeForNextIntervalUpdate;
				}
			}

			end=std::chrono::steady_clock::now();
			time_unit elapsed=std::chrono::duration_cast<time_unit>(end - start);

			if(elapsed < timeForNextUpdate){
				// Increment timing values for each interval
				for(auto& timing : m_table){
					timing.second.timeSinceLastUpdate+=timeForNextUpdate;
				}

				//Wait until next update
				m_cond.wait_for(lock, timeForNextUpdate-elapsed);
			}else{
				// Increment timing values for each interval
				for(auto& timing : m_table)
					timing.second.timeSinceLastUpdate+=elapsed;
			}
		}else
			m_cond.wait(lock);
	}
}
