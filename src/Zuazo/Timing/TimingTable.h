#pragma once

#include <sys/types.h>
#include <chrono>
#include <map>
#include <set>
#include <utility>
#include <memory>

#include "../Updateable.h"
#include "TimeInterval.h"

namespace Zuazo::Timing{

template <u_int32_t order>
class PeriodicUpdate;
template <u_int32_t order>
class RegularUpdate;

class TimingTable{
public:
	using OrderedUpdates=std::map<u_int32_t, std::set<const Updateable*>>;

	struct UpdateInterval{
		OrderedUpdates								updateables;
		TimeInterval								timeSinceLastUpdate;
	};

	struct PendingUpdates{
		OrderedUpdates								updateables;
		TimeInterval								timeForNextUpdate;

		PendingUpdates() :
			timeForNextUpdate(TimeInterval::max())
		{
		}
	};

	TimingTable()=default;
	TimingTable(const TimingTable&)=default;
	~TimingTable()=default;

	template <u_int32_t order>
	void 										addTiming(const PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										addTiming(const RegularUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const RegularUpdate<order>* event);

	PendingUpdates								getPendingUpdates();
	void										incrTime(const TimeInterval& t);
private:
	using RegularTimings=OrderedUpdates;
	using PeriodicTimings=std::map<TimeInterval, UpdateInterval>;

	PeriodicTimings								m_periodicTimings;
	RegularTimings								m_regularTimings;

	void										cleanUnusedIntervals();
};

/*
 * Inline function definitions
 */

inline void	TimingTable::incrTime(const TimeInterval& t){
	//Add the time to the events
	for(auto& timing : m_periodicTimings){
		timing.second.timeSinceLastUpdate += t;
	}
}

template <u_int32_t order>
void TimingTable::addTiming(const PeriodicUpdate<order>* event){
	const TimeInterval& interval=event->getInterval();

	if(interval.count()>0){
		m_periodicTimings[interval].updateables[order].insert(event);

		//Restart all the timings
		for(auto& interval : m_periodicTimings){
			interval.second.timeSinceLastUpdate=interval.first;
		}
	}
}

template <u_int32_t order>
void TimingTable::addTiming(const RegularUpdate<order>* event){
	m_regularTimings[order].insert(event);
}

template <u_int32_t order>
void TimingTable::deleteTiming(const PeriodicUpdate<order>* event){
	for(auto& element : m_periodicTimings)
		element.second.updateables[order].erase(event);
	cleanUnusedIntervals();
}

template <u_int32_t order>
void TimingTable::deleteTiming(const RegularUpdate<order>* event){
	m_regularTimings[order].erase(event);
}

} /* namespace Zuazo::Timing*/

