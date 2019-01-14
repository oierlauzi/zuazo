#pragma once

#include <sys/types.h>
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "Utils/Rational.h"

namespace Zuazo{

typedef std::chrono::microseconds time_unit;
const u_int64_t TIME_UNITS_PER_SECOND = time_unit( std::chrono::seconds(1) ).count();

class Timing{
public:

	/*
	 * Stores all timing values and calls them when time arrives
	 */
	class TimingTable{
	public:
		TimingTable();
		TimingTable(const TimingTable& other)=delete;
		~TimingTable();

		void 					addTiming(Timing* element, time_unit interval);
		void					modifyTiming(Timing* element, time_unit interval);
		void 					deleteTiming(Timing* element);

		time_unit				getCurrentTimeStamp() const;
	private:
		struct TimingData{
			time_unit			timeSinceLastUpdate;
			std::set<Timing*>	elements;
		};

		std::map<time_unit, TimingData> m_table;

		std::thread 			m_thread;
		bool					m_exit;
		std::mutex 				m_mutex;
		std::mutex 				m_timeStampMutex;
		std::condition_variable	m_cond;

		time_unit				m_currTs;

		void					updateThreadFunc();
	};

	const Rational& 	getInterval() const;
	Rational			getRate() const;

	static const TimingTable& getTimingTable();
protected:
	Timing();
	Timing(const Rational& rate);
	Timing(const Timing& other);
	virtual ~Timing();

	void 				setInterval(const Rational& interval);
	void				setRate(const Rational& rate);

	virtual void 		update(const time_unit& elapsed, const time_unit& ts)=0;
	static time_unit	now();

	static time_unit	toTimeUnit(const Rational& rat);
	static Rational 	toRat(const time_unit& clk);

	std::mutex			m_mutex;
private:
	Rational			m_updateInterval;

	static TimingTable	s_timings;

	/*
	 * Useful functions
	 */
};



/*
 * INLINE FUNCTION DECLARATIONS
 */


inline const Rational& Timing::getInterval() const {
	return m_updateInterval;
}

inline Rational Timing::getRate() const {
	return 1/m_updateInterval;
}

inline const Timing::TimingTable& Timing::getTimingTable(){
	return s_timings;
}



inline time_unit Timing::now(){
	return s_timings.getCurrentTimeStamp();
}



inline time_unit Timing::toTimeUnit(const Rational& rat){
	if(rat.num && rat.den)
		return time_unit(rat.num * TIME_UNITS_PER_SECOND / rat.den);
	else
		return time_unit(0);
}
inline Zuazo::Rational Timing::toRat(const time_unit& clk){
	return Rational(clk.count(), TIME_UNITS_PER_SECOND);
}



inline time_unit Timing::TimingTable::getCurrentTimeStamp() const{
	std::unique_lock<std::mutex> lock(
			const_cast<TimingTable*>(this)->m_timeStampMutex
			);
	return m_currTs;
}
}
