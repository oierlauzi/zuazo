#include "Timing.h"

#include <chrono>
#include <cstdio>
#include <set>

#include "Chronometer.h"
#include "PeriodicUpdate.h"
#include "RegularUpdate.h"
#include "TimeInterval.h"

#include "../Graphics/Context.h"

namespace Zuazo::Timing{

/*
 * Static variables
 */
std::unique_ptr<Timings> timings;


/*
 * Timings' functions
 */

Timings::Timings(){
	m_exit=false;
	m_currTime=TimePoint::now();
	m_thread=std::thread(&Timings::threadFunc, this);
}

Timings::~Timings(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}

void Timings::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){

		#ifdef ENABLE_LOAD_DEBUG
		Chronometer chrono;
		chrono.start();
		#endif

		TimingTable::PendingUpdates updates=m_timingTable.getPendingUpdates();

		if(updates.periodicUpdates.first.size() || updates.periodicUpdates.last.size()){
			//Make all the needed updates
			Graphics::UniqueContext ctx(Graphics::Context::mainCtx);

			//Do all regular updates with first priority
			for(auto& regularEvent : updates.regularUpdates.first){
				regularEvent->perform();
			}

			//Do all peridodic updates with first priority
			for(auto& periodicEvent : updates.periodicUpdates.first){
				periodicEvent->perform();
			}

			//Do all regular updates with last priority
			for(auto& regularEvent : updates.regularUpdates.last){
				regularEvent->perform();
			}

			//Do all peridodic updates with last priority
			for(auto& periodicEvent : updates.periodicUpdates.last){
				periodicEvent->perform();
			}


			// Increment timing values for each interval and for currTime
			m_currTime+=updates.timeForNextUpdate;
			m_timingTable.addTime(updates.timeForNextUpdate);

			//Wait until next update
			m_cond.wait_until(lock, m_currTime);
		}else{
			m_cond.wait(lock);
		}

		#ifdef ENABLE_LOAD_DEBUG
		chrono.end();
		printf("Elapsed: %ldus\n", chrono.getElapsed().count());
		#endif
	}
}

/*
 * init() and end()
 */
int init(){
	timings=std::unique_ptr<Timings>(new Timings());
	return 0;
}

int end(){
	timings=std::unique_ptr<Timings>();
	return 0;
}

}
