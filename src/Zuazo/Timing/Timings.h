#pragma once

#include <sys/types.h>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "../Graphics/Context.h"
#include "UpdateableBase.h"
#include "../Utils/TimeInterval.h"
#include "../Utils/TimePoint.h"

//#define ENABLE_LOAD_DEBUG

namespace Zuazo::Timing{

template <u_int32_t order>
class PeriodicUpdate;
template <u_int32_t order>
class RegularUpdate;

class Timings{
public:
	Timings();
	Timings(const Timings& other)=delete;
	~Timings();

	const Utils::TimePoint&						now() const;
	const Utils::TimeInterval&					getElapsed() const;

	void										lock();
	void										unlock();
	bool										try_lock();

	template <u_int32_t order>
	void 										addTiming(const PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										addTiming(const RegularUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const PeriodicUpdate<order>* event);
	template <u_int32_t order>
	void 										deleteTiming(const RegularUpdate<order>* event);
	template <u_int32_t order>
	void 										modifyTiming(const PeriodicUpdate<order>* event);
private:

	class TimingTable{
	public:
		using OrderedUpdates=std::map<u_int32_t, std::set<const UpdateableBase*>>;

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
		void 										addTiming(const PeriodicUpdate<order>* event);
		template <u_int32_t order>
		void 										addTiming(const RegularUpdate<order>* event);
		template <u_int32_t order>
		void 										deleteTiming(const PeriodicUpdate<order>* event);
		template <u_int32_t order>
		void 										deleteTiming(const RegularUpdate<order>* event);

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
	Utils::TimeInterval							m_elapsed;

	std::thread 								m_thread;
	bool										m_exit;
	std::mutex 									m_mutex;
	std::condition_variable						m_cond;

	void										threadFunc();
};

/*
 * TIMING'S INLINE METHOD DEFINITIONS
 */

inline const Utils::TimePoint& Timings::now() const{
	return m_currTime;
}

inline const Utils::TimeInterval& Timings::getElapsed() const{
	return m_elapsed;
}



inline void Timings::lock(){
	m_mutex.lock();
}

inline void Timings::unlock(){
	m_mutex.unlock();
}

inline bool Timings::try_lock(){
	return m_mutex.try_lock();
}



template <u_int32_t order>
inline void Timings::addTiming(const PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

template <u_int32_t order>
inline void Timings::addTiming(const RegularUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
}

template <u_int32_t order>
inline void Timings::deleteTiming(const PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

template <u_int32_t order>
inline void Timings::deleteTiming(const RegularUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

template <u_int32_t order>
inline void Timings::modifyTiming(const PeriodicUpdate<order>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

/*
 * TIMING TABLE'S INLINE METHOD DEFINITIONS
 */

template <u_int32_t order>
inline void Timings::TimingTable::addTiming(const PeriodicUpdate<order>* event){
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
inline void Timings::TimingTable::addTiming(const RegularUpdate<order>* event){
	m_regularTimings[order].insert(event);
}

template <u_int32_t order>
inline void Timings::TimingTable::deleteTiming(const PeriodicUpdate<order>* event){
	for(auto& element : m_periodicTimings)
		element.second.updateables[order].erase(event);
	cleanUnusedIntervals();
}

template <u_int32_t order>
inline void Timings::TimingTable::deleteTiming(const RegularUpdate<order>* event){
	m_regularTimings[order].erase(event);
	cleanOrderedUpdates(&m_regularTimings);
}
}
