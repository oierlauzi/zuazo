#include <Timing.h>

#include <Graphics/Context.h>

using namespace Zuazo;

std::unique_ptr<Timing> Timing::s_timing;

/*
 * TIMING'S METHOD DEFINITIONS
 */

Timing::Timing() :
	m_currTime(std::chrono::steady_clock::now()),
	m_elapsed(Duration::zero())
{
	m_exit=false;
	m_thread=std::thread(&Timing::threadFunc, this);
}

Timing::~Timing(){
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_exit=true;
        m_cond.notify_all();
    }
	m_thread.join();
}



void Timing::addTiming(const PeriodicUpdate* event){
	//Get information about the event
	Duration interval=event->getInterval();

	if(interval.count()){
		//Get the priorty for update ordering
		UpdateableBase::UpdatePriority prior=event->getPriority();

		auto ite=m_periodicUpdates.find(interval);
		if(ite == m_periodicUpdates.end()){
			//Update interval did not exist. Create it
			m_periodicUpdates[interval].periodicUpdates[prior].insert(event);

			//Restart all the timings
			m_cond.notify_all();
			for(auto& update : m_periodicUpdates){
				update.second.timeSinceLastUpdate=update.first;
			}
		}else{
			//Update interval exists. Simply add the new update
			ite->second.periodicUpdates[prior].insert(event);
		}
	}
}

void Timing::addTiming(const RegularUpdate* event){
	//Get the priorty for update ordering
	UpdateableBase::UpdatePriority prior=event->getPriority();

	//Insert it at the proper place
	m_regularUpdates[prior].insert(event);
}

void Timing::deleteTiming(const PeriodicUpdate* event){
	//Get information about the event
	Duration interval=event->getInterval();

	auto intervalIte=m_periodicUpdates.find(interval);
	if(intervalIte != m_periodicUpdates.end()){
		//Get the priorty for update ordering
		UpdateableBase::UpdatePriority prior=event->getPriority();

		auto priorityIte=intervalIte->second.periodicUpdates.find(prior);
		if(priorityIte != intervalIte->second.periodicUpdates.end()){
			//Desired set was found
			priorityIte->second.erase(event); //Try to erase the event

			//Delete the set if it has no events in it
			if(priorityIte->second.empty()){
				intervalIte->second.periodicUpdates.erase(priorityIte);
			}
		}

		//Delete the map if it has no event sets in it
		if(intervalIte->second.periodicUpdates.empty()){
			m_periodicUpdates.erase(intervalIte);
		}
	}
}

void Timing::deleteTiming(const RegularUpdate* event){
	//Get the priorty for update ordering
	UpdateableBase::UpdatePriority prior=event->getPriority();

	auto priorityIte=m_regularUpdates.find(prior);
	if(priorityIte != m_regularUpdates.end()){
		//Desired set was found
		priorityIte->second.erase(event); //Try to erase the event

		//Delete the set if it has no events in it
		if(priorityIte->second.empty()){
			m_regularUpdates.erase(priorityIte);
		}
	}
}

void Timing::modifyTiming(const PeriodicUpdate* event){
	deleteTiming(event);
	addTiming(event);
}

void Timing::threadFunc() const{
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){
		OrderedUpdates pendingUpdates;
		Duration timeForNextUpdate(Duration::max());

		//Get all the pending updates
		for(auto& updateInterval : m_periodicUpdates){
			auto& timeInterval		=updateInterval.first;
			auto& intervalUpdates	=updateInterval.second;

			if(intervalUpdates.timeSinceLastUpdate >= timeInterval){
				//This interval needs to be updated
				for(auto& updateOrder : intervalUpdates.periodicUpdates){
					pendingUpdates[updateOrder.first].insert(updateOrder.second.begin(), updateOrder.second.end());
				}

				//Decrement the time
				intervalUpdates.timeSinceLastUpdate-=timeInterval;
			}

			//Calculate time for next update.
			const Duration timeForNextIntervalUpdate=timeInterval - intervalUpdates.timeSinceLastUpdate;
			//If it is sooner than the previous one, override it
			if(timeForNextIntervalUpdate < timeForNextUpdate){
				//This interval's update is the soonest one
				timeForNextUpdate=timeForNextIntervalUpdate;
			}
		}

		if(pendingUpdates.size()){
			//Append regular updates (NonLinear ones are included here)
			for(auto& updateOrder : m_regularUpdates){
				pendingUpdates[updateOrder.first].insert(updateOrder.second.begin(), updateOrder.second.end());
			}

			//Update them
			{
				Graphics::UniqueContext ctx(*Graphics::Context::getMainCtx());
				for(auto& priority : pendingUpdates){
					for(auto& updateable : priority.second){
						updateable->update();
					}
				}
			}

			//Update all the timing data
			m_currTime+=timeForNextUpdate;
			m_elapsed=timeForNextUpdate;
			for(auto& interval : m_periodicUpdates){
				interval.second.timeSinceLastUpdate+=timeForNextUpdate;
			}

			//Wait until next update
			m_cond.wait_until(lock, m_currTime);
		}else{
			//Wait until an event is added
			m_cond.wait(lock);
		}
	}
}
