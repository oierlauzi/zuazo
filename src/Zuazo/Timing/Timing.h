#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "TimingTable.h"
#include "TimePoint.h"
#include "UpdateOrder.h"

//#define ENABLE_LOAD_DEBUG

namespace Zuazo::Timing{

class Timings{
public:
	Timings();
	Timings(const Timings& other)=delete;
	~Timings();

	TimePoint									now();

	void										lock();
	void										unlock();
	bool										try_lock();

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




inline void Timings::addTiming(PeriodicUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

inline void Timings::addTiming(PeriodicUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

inline void Timings::addTiming(RegularUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
}

inline void Timings::addTiming(RegularUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.addTiming(event);
}

inline void Timings::deleteTiming(PeriodicUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

inline void Timings::deleteTiming(PeriodicUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

inline void Timings::deleteTiming(RegularUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

inline void Timings::deleteTiming(RegularUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
}

inline void Timings::modifyTiming(PeriodicUpdate<UpdateOrder::FIRST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

inline void Timings::modifyTiming(PeriodicUpdate<UpdateOrder::LAST>* event){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_timingTable.deleteTiming(event);
	m_timingTable.addTiming(event);
	m_cond.notify_one();
}

}
