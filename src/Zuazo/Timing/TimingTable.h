#pragma once

#include <map>
#include <set>

#include "TimeUnit.h"
#include "UpdateOrder.h"

namespace Zuazo::Timing{

template <UpdateOrder TPriority>
class PeriodicUpdate;

template <UpdateOrder TPriority>
class RegularUpdate;

class TimingTable{
public:
	struct RegularUpdates{
		std::set<RegularUpdate<UpdateOrder::FIRST>*>	first;
		std::set<RegularUpdate<UpdateOrder::LAST>*>		last;
	};

	struct PeriodicUpdates{
		std::set<PeriodicUpdate<UpdateOrder::FIRST>*>	first;
		std::set<PeriodicUpdate<UpdateOrder::LAST>*>	last;
	};

	struct UpdateInterval{
		PeriodicUpdates									updates;
		TimeInterval										timeSinceLastUpdate;
	};

	struct PendingUpdates{
		TimeInterval										timeForNextUpdate;
		const RegularUpdates&							regularUpdates;
		PeriodicUpdates									periodicUpdates;
	};

	TimingTable()=default;
	TimingTable(const TimingTable&)=default;
	~TimingTable()=default;

	void 										addTiming(PeriodicUpdate<UpdateOrder::FIRST>* event);
	void 										addTiming(PeriodicUpdate<UpdateOrder::LAST>* event);
	void 										addTiming(RegularUpdate<UpdateOrder::FIRST>* event);
	void 										addTiming(RegularUpdate<UpdateOrder::LAST>* event);
	void 										deleteTiming(PeriodicUpdate<UpdateOrder::FIRST>* event);
	void 										deleteTiming(PeriodicUpdate<UpdateOrder::LAST>* event);
	void 										deleteTiming(RegularUpdate<UpdateOrder::FIRST>* event);
	void 										deleteTiming(RegularUpdate<UpdateOrder::LAST>* event);

	PendingUpdates								getPendingUpdates();
	void										addTime(const TimeInterval& t);
private:
	using RegularTimings=RegularUpdates;
	using PeriodicTimings=std::map<TimeInterval, UpdateInterval>;

	void										addInterval(const TimeInterval& interval);
	void 										delUnusedInterval();

	PeriodicTimings								m_periodicTimings;
	RegularTimings								m_regularTimings;

};

/*
 * Inline function definitions
 */

inline void	TimingTable::addTime(const TimeInterval& t){
	//Add the time to the events
	for(auto& timing : m_periodicTimings){
		timing.second.timeSinceLastUpdate+=t;
	}
}

} /* namespace Zuazo::Timing*/

