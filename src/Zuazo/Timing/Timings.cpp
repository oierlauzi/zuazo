#include "Timings.h"

#include <chrono>
#include <utility>

namespace Zuazo::Timing{

/*
 * TIMING'S METHOD DEFINITIONS
 */

Timings::Timings(){
	m_exit=false;
	m_currTime=Utils::TimePoint::now();
	m_thread=std::thread(&Timings::threadFunc, this);
}

Timings::~Timings(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}

void Timings::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);
	Graphics::UniqueContext ctx(m_ctx);

	while(!m_exit){

		#ifdef ENABLE_LOAD_DEBUG
		Chronometer chrono;
		chrono.start();
		#endif

		TimingTable::PendingUpdates updates=m_timingTable.getPendingUpdates();

		if(updates.updateables.size()){
			//Make all the needed updates

			//Sort by update priority
			//std::sort(updates.updateables.begin(), updates.updateables.end());
			/*
			 * No need, its actually sorted by default
			 */

			//Update all
			for(auto ite=updates.updateables.begin(); ite!=updates.updateables.end(); ++ite){
				for(const UpdateableBase * updateable : ite->second){
					std::lock_guard<const UpdateableBase> lock(*updateable);
					updateable->update();
				}
			}

			// Increment timing values for each interval and for currTime
			m_currTime+=updates.timeForNextUpdate;
			m_elapsed=updates.timeForNextUpdate;
			m_timingTable.incrTime(updates.timeForNextUpdate);

			//Wait until next update
			lock.unlock();
			std::this_thread::sleep_until(m_currTime);
			lock.lock();
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
 * TIMING TABLE'S METHOD DEFINITIONS
 */

Timings::TimingTable::PendingUpdates	Timings::TimingTable::getPendingUpdates(){
	PendingUpdates pend;

	for(auto& timing : m_periodicTimings){
		const Utils::TimeInterval& interval=timing.first;
		UpdateInterval& intervalUpdates=timing.second;

		if(intervalUpdates.timeSinceLastUpdate >= interval){
			//This interval needs to be updated
			for(auto& updateOrder : intervalUpdates.updateables){
				pend.updateables[updateOrder.first].insert(updateOrder.second.begin(), updateOrder.second.end());
			}

			//Decrement the time
			intervalUpdates.timeSinceLastUpdate-=interval;
		}

		//Calculate time for next update.
		const Utils::TimeInterval timeForNextUpdate=interval - intervalUpdates.timeSinceLastUpdate;
		//If it is sooner than the previous one, override it
		if(timeForNextUpdate < pend.timeForNextUpdate){
			//This interval's update is the soonest one
			pend.timeForNextUpdate=timeForNextUpdate;
		}
	}

	if(pend.updateables.size()){
		//Insert regular updates
		for(auto& updateOrder : m_regularTimings){
			pend.updateables[updateOrder.first].insert(updateOrder.second.begin(), updateOrder.second.end());
		}
	}

	return pend;
}

void Timings::TimingTable::incrTime(const Utils::TimeInterval& t){
	//Add the time to the events
	for(auto& timing : m_periodicTimings){
		timing.second.timeSinceLastUpdate += t;
	}
}

void Timings::TimingTable::cleanUnusedIntervals(){
	auto ite=m_periodicTimings.begin();

	while(ite != m_periodicTimings.end()){
		cleanOrderedUpdates(&(ite->second.updateables));

		if(ite->second.updateables.size()){
			++ite;
		}else{
			ite=m_periodicTimings.erase(ite);
		}

	}
}

void Timings::TimingTable::cleanOrderedUpdates(OrderedUpdates* updates){
	auto ite=updates->begin();

	while(ite != updates->end()){
		if(ite->second.size()){
			//Update order is being used
			++ite;
		} else{
			//Update order is NOT being used
			ite=updates->erase(ite);
		}
	}
}

}
