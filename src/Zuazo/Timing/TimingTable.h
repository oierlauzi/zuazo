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
		INPUT,
		CONSUMER
	};

	template <UpdatePriority TPriority>
	class Periodic : Updateable{
		friend TimingTable;
	public:
		Periodic();
		Periodic(const Utils::Rational& rate);
		Periodic(const Periodic& other);
		virtual ~Periodic();

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
private:
	struct UpdateInterval{
		std::set<Periodic<UpdatePriority::INPUT>*>		inputs;
		std::set<Periodic<UpdatePriority::CONSUMER>*>	outputs;
		TimeUnit										timeSinceLastUpdate;
	};

	void 										addTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval);
	void 										addTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval);
	void 										deleteTiming(Periodic<UpdatePriority::INPUT>* event);
	void 										deleteTiming(Periodic<UpdatePriority::CONSUMER>* event);
	void 										modifyTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval);
	void 										modifyTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval);


	void										addInterval(const TimeUnit& interval);
	void 										delUnusedInterval();

	std::map<TimeUnit, UpdateInterval>			m_timings;
	TimePoint									m_currTime;

	std::thread 								m_thread;
	bool										m_exit;
	std::mutex 									m_mutex;
	std::condition_variable						m_cond;

	void										updateThreadFunc();
};

extern std::unique_ptr<TimingTable> 	timings;

/*
 * INLINE METHOD DEFINITIONS
 */

inline TimePoint TimingTable::now(){
	return m_currTime;
}

/*
 * Periodic updates
 */

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::Periodic<TPriority>::Periodic() : Updateable(){

}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::Periodic<TPriority>::Periodic(const Utils::Rational& rate) : Updateable(){
	setRate(rate);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::Periodic<TPriority>::Periodic(const Periodic& other) : Updateable(other){
	setInterval(other.m_updateInterval);
}

template <TimingTable::UpdatePriority TPriority>
inline TimingTable::Periodic<TPriority>::~Periodic(){
	if(timings)
		timings->deleteTiming(this);
}



template <TimingTable::UpdatePriority TPriority>
inline const Utils::Rational& TimingTable::Periodic<TPriority>::getInterval() const {
	return m_updateInterval;
}

template <TimingTable::UpdatePriority TPriority>
inline Utils::Rational TimingTable::Periodic<TPriority>::getRate() const {
	return 1/m_updateInterval;
}


template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::Periodic<TPriority>::open(){
	Updateable::open();
	timings->addTiming(this, m_updateInterval);
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::Periodic<TPriority>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::Periodic<TPriority>::setInterval(const Utils::Rational& interval){
	m_updateInterval=interval;

	if(isOpen()){
		TimeUnit tuInterval(m_updateInterval);
		timings->modifyTiming(this, tuInterval);
	}
}

template <TimingTable::UpdatePriority TPriority>
inline void TimingTable::Periodic<TPriority>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}
} /* namespace Zuazo::Timing*/

