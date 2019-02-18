#include "TimingTable.h"

#include "TimeInterval.h"

using namespace Zuazo::Timing;

TimingTable::PendingUpdates	TimingTable::getPendingUpdates(){
	PendingUpdates pend;

	for(auto& timing : m_periodicTimings){
		const TimeInterval& interval=timing.first;
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
		const TimeInterval timeForNextUpdate=interval - intervalUpdates.timeSinceLastUpdate;
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

void TimingTable::cleanUnusedIntervals(){
	auto ite=m_periodicTimings.begin();

	while(ite != m_periodicTimings.end()){
		bool intervalIsEmpty=true;

		for(auto& updateOrder : ite->second.updateables){
			if(updateOrder.second.size()){
				//There is something
				intervalIsEmpty=false;
				break;
			}
		}

		if(intervalIsEmpty){
			ite=m_periodicTimings.erase(ite);
		}else
			++ite;
	}
}
