#pragma once

#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "../Updateable.h"
#include "../Utils/Rational.h"
#include "TimePoint.h"
#include "TimeUnit.h"

#define ENABLE_LOAD_DEBUG

namespace Zuazo::Timing{

class TimingTable{
public:
	TimingTable();
	TimingTable(const TimingTable&)=delete;
	~TimingTable();

	TimePoint								now();

	enum class UpdatePriority{
		FIRST,
		LAST
	};

	template <UpdatePriority TPriority>
	class PeriodicEvent : public Updateable{
		friend TimingTable;
	public:
		PeriodicEvent();
		PeriodicEvent(const Utils::Rational& rate);
		PeriodicEvent(const PeriodicEvent& other);
		virtual ~PeriodicEvent();

		const Utils::Rational& 				getInterval() const;
		Utils::Rational						getRate() const;

		virtual void						open() override;
		virtual void						close() override;
	protected:
		void 								setInterval(const Utils::Rational& interval);
		void								setRate(const Utils::Rational& rate);
	private:
		Utils::Rational						m_updateInterval;
	};

	template <UpdatePriority TPriority>
	class RegularEvent : public Updateable{
		friend TimingTable;
	public:
		RegularEvent();
		RegularEvent(const RegularEvent& other);
		virtual ~RegularEvent();

		virtual void						open() override;
		virtual void						close() override;
	};
private:
	struct UpdateInterval{
		std::set<PeriodicEvent<UpdatePriority::FIRST>*>	first;
		std::set<PeriodicEvent<UpdatePriority::LAST>*>	last;
		TimeUnit										timeSinceLastUpdate;
	};

	using PeriodicTimings=std::map<TimeUnit, UpdateInterval>;

	struct RegularTimings{
		std::set<RegularEvent<UpdatePriority::FIRST>*>	first;
		std::set<RegularEvent<UpdatePriority::LAST>*>	last;
	};

	void 										addTiming(PeriodicEvent<UpdatePriority::FIRST>* event, const TimeUnit& interval);
	void 										addTiming(PeriodicEvent<UpdatePriority::LAST>* event, const TimeUnit& interval);
	void 										addTiming(RegularEvent<UpdatePriority::FIRST>* event);
	void 										addTiming(RegularEvent<UpdatePriority::LAST>* event);
	void 										deleteTiming(PeriodicEvent<UpdatePriority::FIRST>* event);
	void 										deleteTiming(PeriodicEvent<UpdatePriority::LAST>* event);
	void 										deleteTiming(RegularEvent<UpdatePriority::FIRST>* event);
	void 										deleteTiming(RegularEvent<UpdatePriority::LAST>* event);
	void 										modifyTiming(PeriodicEvent<UpdatePriority::FIRST>* event, const TimeUnit& interval);
	void 										modifyTiming(PeriodicEvent<UpdatePriority::LAST>* event, const TimeUnit& interval);


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

extern std::unique_ptr<TimingTable> 		timings;

/*
 * INLINE METHOD DEFINITIONS
 */

inline TimePoint TimingTable::now(){
	return m_currTime;
}

/*
 * PeriodicEvent methods
 */

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::PeriodicEvent<TPriority>::PeriodicEvent() : Updateable(){

}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::PeriodicEvent<TPriority>::PeriodicEvent(const Utils::Rational& rate) : Updateable(){
	setRate(rate);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::PeriodicEvent<TPriority>::PeriodicEvent(const PeriodicEvent& other) : Updateable(other){
	setInterval(other.m_updateInterval);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::PeriodicEvent<TPriority>::~PeriodicEvent(){
	if(timings)
		timings->deleteTiming(this);
}



template <TimingTable::UpdatePriority TPriority>
inline const Utils::Rational& TimingTable::PeriodicEvent<TPriority>::getInterval() const {
	return m_updateInterval;
}

template <TimingTable::UpdatePriority TPriority>
inline Utils::Rational TimingTable::PeriodicEvent<TPriority>::getRate() const {
	return 1/m_updateInterval;
}


template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::PeriodicEvent<TPriority>::open(){
	Updateable::open();
	timings->addTiming(this, m_updateInterval);
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::PeriodicEvent<TPriority>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::PeriodicEvent<TPriority>::setInterval(const Utils::Rational& interval){
	m_updateInterval=interval;

	if(isOpen()){
		TimeUnit tuInterval(m_updateInterval);
		timings->modifyTiming(this, tuInterval);
	}
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::PeriodicEvent<TPriority>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}

/*
 * RegularEvent Methods
 */

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::RegularEvent<TPriority>::RegularEvent(){
	timings->addTiming(this);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::RegularEvent<TPriority>::RegularEvent(const RegularEvent& other){
	timings->addTiming(this);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::RegularEvent<TPriority>::~RegularEvent(){
	if(timings)
		timings->deleteTiming(this);
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::RegularEvent<TPriority>::open(){
	Updateable::open();
	timings->addTiming(this);
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::RegularEvent<TPriority>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}

/*
 * TimingTable methods
 */
} /* namespace Zuazo::Timing*/

