#pragma once

#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "TimePoint.h"
#include "TimeUnit.h"
#include "UpdateOrder.h"

#define ENABLE_LOAD_DEBUG

namespace Zuazo::Timing{

template <UpdateOrder TPriority>
class PeriodicUpdate;

template <UpdateOrder TPriority>
class RegularUpdate;

class TimingTable{
public:
	struct UpdateInterval{
		std::set<PeriodicUpdate<UpdateOrder::FIRST>*> first;
		std::set<PeriodicUpdate<UpdateOrder::LAST>*>	last;
		TimeUnit										timeSinceLastUpdate;
	};

	using PeriodicTimings=std::map<TimeUnit, UpdateInterval>;

	struct RegularTimings{
		std::set<RegularUpdate<UpdateOrder::FIRST>*>	first;
		std::set<RegularUpdate<UpdateOrder::LAST>*>	last;
	};

	TimingTable();
	TimingTable(const TimingTable&)=delete;
	~TimingTable();

	TimePoint									now();

	void 										addTiming(PeriodicUpdate<UpdateOrder::FIRST>* event);
	void 										addTiming(PeriodicUpdate<UpdateOrder::LAST>* event);
	void 										addTiming(RegularUpdate<UpdateOrder::FIRST>* event);
	void 										addTiming(RegularUpdate<UpdateOrder::LAST>* event);
	void 										deleteTiming(PeriodicUpdate<UpdateOrder::FIRST>* event);
	void 										deleteTiming(PeriodicUpdate<UpdateOrder::LAST>* event);
	void 										deleteTiming(RegularUpdate<UpdateOrder::FIRST>* event);
	void 										deleteTiming(RegularUpdate<UpdateOrder::LAST>* event);
	void 										modifyTiming(PeriodicUpdate<UpdateOrder::FIRST>* event);
	void 										modifyTiming(PeriodicUpdate<UpdateOrder::LAST>* event);

private:
	void										addInterval(const TimeUnit& interval);
	void 										delUnusedInterval();

	PeriodicTimings								m_periodicTimings;
	RegularTimings								m_regularTimings;

	TimePoint									m_currTime;

	std::thread 								m_thread;
	bool										m_exit;
	std::mutex 									m_mutex;
	std::condition_variable						m_cond;

	void										updateThreadFunc();
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline TimePoint TimingTable::now(){
	return m_currTime;
}
} /* namespace Zuazo::Timing*/

