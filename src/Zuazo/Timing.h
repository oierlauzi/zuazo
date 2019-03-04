#pragma once

#include <sys/types.h>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "Graphics/Context.h"
#include "Updateables/Updateable.h"
#include "Utils/TimeInterval.h"
#include "Utils/TimePoint.h"

//#define ENABLE_LOAD_DEBUG

namespace Zuazo{
namespace Updateables{
template <u_int32_t order>
class PeriodicUpdate;
template <u_int32_t order>
class RegularUpdate;
}

class Timing{
public:
	Timing();
	Timing(const Timing& other)=delete;
	~Timing();

	Utils::TimePoint							now();

	void										lock();
	void										unlock();
	bool										try_lock();

	template <u_int32_t order>
	void 										addTiming(const Updateables::PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										addTiming(const Updateables::RegularUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const Updateables::PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const Updateables::RegularUpdate<order>* event);
	template <u_int32_t order>
	void 										modifyTiming(const Updateables::PeriodicUpdate<order>* event);
private:

	class TimingTable{
	public:
		using OrderedUpdates=std::map<u_int32_t, std::set<const Updateables::Updateable*>>;

		struct UpdateInterval{
			OrderedUpdates								updateables;
			Utils::TimeInterval							timeSinceLastUpdate;
		};

		struct PendingUpdates{
			OrderedUpdates								updateables;
			Utils::TimeInterval							timeForNextUpdate;

			PendingUpdates() :
				timeForNextUpdate(Utils::TimeInterval::max())
			{
			}
		};

		TimingTable()=default;
		TimingTable(const TimingTable&)=default;
		~TimingTable()=default;

		template <u_int32_t order>
		void 										addTiming(const Updateables::PeriodicUpdate<order>* event);
		template <u_int32_t order>
		void 										addTiming(const Updateables::RegularUpdate<order>* event);
		template <u_int32_t order>
		void 										deleteTiming(const Updateables::PeriodicUpdate<order>* event);
		template <u_int32_t order>
		void 										deleteTiming(const Updateables::RegularUpdate<order>* event);

		PendingUpdates								getPendingUpdates();
		void										incrTime(const Utils::TimeInterval& t);
	private:
		using RegularTimings=OrderedUpdates;
		using PeriodicTimings=std::map<Utils::TimeInterval, UpdateInterval>;

		PeriodicTimings								m_periodicTimings;
		RegularTimings								m_regularTimings;

		void										cleanUnusedIntervals();
		static void									cleanOrderedUpdates(OrderedUpdates* updates);
	};

	TimingTable									m_timingTable;

	Graphics::Context							m_ctx;

	Utils::TimePoint							m_currTime;

	std::thread 								m_thread;
	bool										m_exit;
	std::mutex 									m_mutex;
	std::condition_variable						m_cond;

	void										threadFunc();
};

/*
 * TIMING'S INLINE METHOD DEFINITIONS
 */

inline Utils::TimePoint Timing::now(){
	return m_currTime;
}




inline void Timing::lock(){
	m_mutex.lock();
}

inline void Timing::unlock(){
	m_mutex.unlock();
}

inline bool Timing::try_lock(){
	return m_mutex.try_lock();
}



template <u_int32_t order>
inline void Timing::addTiming(const Updateables::PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

template <u_int32_t order>
inline void Timing::addTiming(const Updateables::RegularUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
}

template <u_int32_t order>
inline void Timing::deleteTiming(const Updateables::PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

template <u_int32_t order>
inline void Timing::deleteTiming(const Updateables::RegularUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

template <u_int32_t order>
inline void Timing::modifyTiming(const Updateables::PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

/*
 * TIMING TABLE'S INLINE METHOD DEFINITIONS
 */

template <u_int32_t order>
inline void Timing::TimingTable::addTiming(const Updateables::PeriodicUpdate<order>* event){
	const Utils::TimeInterval& interval=event->getInterval();

	if(interval.count()>0){
		m_periodicTimings[interval].updateables[order].insert(event);

		//Restart all the timings
		for(auto& interval : m_periodicTimings){
			interval.second.timeSinceLastUpdate=interval.first;
		}
	}
}

template <u_int32_t order>
inline void Timing::TimingTable::addTiming(const Updateables::RegularUpdate<order>* event){
	m_regularTimings[order].insert(event);
}

template <u_int32_t order>
inline void Timing::TimingTable::deleteTiming(const Updateables::PeriodicUpdate<order>* event){
	for(auto& element : m_periodicTimings)
		element.second.updateables[order].erase(event);
	cleanUnusedIntervals();
}

template <u_int32_t order>
inline void Timing::TimingTable::deleteTiming(const Updateables::RegularUpdate<order>* event){
	m_regularTimings[order].erase(event);
	cleanOrderedUpdates(&m_regularTimings);
}
}
