#pragma once

#include <sys/types.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "TimePoint.h"
#include "TimingTable.h"

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

	TimePoint									now();

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
	TimingTable									m_timingTable;

	TimePoint									m_currTime;

	std::thread 								m_thread;
	bool										m_exit;
	std::mutex 									m_mutex;
	std::condition_variable						m_cond;

	void										threadFunc();
};


extern std::unique_ptr<Timings> timings;

extern int init();
extern int end();

/*
 * INLINE METHOD DEFINITIONS
 */

inline TimePoint Timings::now(){
	return m_currTime;
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

}
