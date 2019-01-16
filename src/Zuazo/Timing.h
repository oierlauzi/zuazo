#pragma once

#include <bits/stdint-intn.h>
#include <chrono>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <set>

#include "Utils/Rational.h"

namespace Zuazo{

#define ZUAZO_TIME_UNIT std::chrono::microseconds
#define ZUAZO_CLOCK std::chrono::steady_clock
#define ZUAZO_TIME_POINT ZUAZO_CLOCK::time_point

/*
 * Class containing all the timing information
 */
class Timing{
public:
	typedef ZUAZO_TIME_UNIT time_unit;
	typedef ZUAZO_TIME_POINT time_point;
	static const int64_t TIME_UNITS_PER_SECOND= time_unit( std::chrono::seconds(1) ).count();

	/*
	 * Class which defines a periodical event
	 */
	class Updateable{
		friend Timing;
	public:
		/*
		 * An update event's callback function
		 */
		class Callback{
		public:
			virtual void						update(const time_unit& elapsed, const time_point& ts)=0;
		};

		Updateable();
		Updateable(const Rational& rate);
		Updateable(const Updateable& other);
		virtual ~Updateable();
		void								setBeforeUpdateCallback(Callback * cbk);
		void								setAfterUpdateCallback(Callback * cbk);

		const Rational& 					getInterval() const;
		Rational							getRate() const;

		bool								isOpen() const;
	protected:
		mutable std::recursive_mutex		m_mutex;

		void 								setInterval(const Rational& interval);
		void								setRate(const Rational& rate);

		virtual void						open();
		virtual void						close();

		virtual void						update(const time_unit& elapsed, const time_point& ts)=0;
	private:
		Rational							m_updateInterval;
		bool								m_isOpen;

		Callback *							m_beforeCbk;
		Callback *							m_afterCbk;

		virtual void						perform(const time_unit& elapsed, const time_point& ts);
	};

	class Chronometer{
	public:
		Chronometer()=default;
		Chronometer(const Chronometer& other)=default;
		~Chronometer()=default;

		void								start();
		void								end();
		time_unit							getElapsed() const;
		const time_point&					getStart() const;
		const time_point&					getEnd() const;

	private:
		time_point							m_start;
		time_point							m_end;
	};

	static int							init();
	static int 							end();

	static time_point					now();
	static Rational 					toRat(const time_unit& tu);
	static time_unit 					toTu(const Rational& rat);
	static time_unit					diff(const time_point& a, const time_point& b);

	Timing()=delete;
	Timing(const Timing&)=delete;
	~Timing()=delete;
private:
	struct UpdateInterval{
		std::set<Updateable *>				updateables;
		time_unit							timeSinceLastUpdate;

		UpdateInterval()=default;
		UpdateInterval(const UpdateInterval& other)=default;
		UpdateInterval(const time_unit& interval) : timeSinceLastUpdate(interval){}
		~UpdateInterval()=default;
	};

	static std::map<time_unit, UpdateInterval> s_timings;
	static std::thread 					s_thread;
	static bool							s_exit;
	static std::mutex 					s_mutex;
	static std::condition_variable		s_cond;

	static void 						addTiming(Updateable* event, const time_unit& interval);
	static void 						deleteTiming(Updateable* event);
	static void 						modifyTiming(Updateable* event, const time_unit& interval);


	static void							updateThreadFunc();
};


/*
 * INLINE METHOD DEFINITIONS
 */

/*
 *  Timing::Updateable methods
 */

inline const Rational& Timing::Updateable::getInterval() const {
	return m_updateInterval;
}

inline Rational Timing::Updateable::getRate() const {
	return 1/m_updateInterval;
}

inline bool Timing::Updateable::isOpen() const{
	return m_isOpen;
}

inline void	Timing::Updateable::perform(const time_unit& elapsed, const time_point& ts){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if(m_beforeCbk)
		m_beforeCbk->update(elapsed, ts);

	update(elapsed, ts);

	if(m_afterCbk)
		m_afterCbk->update(elapsed, ts);
}

inline void Timing::Updateable::open(){
	Timing::addTiming(this, Timing::toTu(getInterval()));
	m_isOpen=true;
}

inline void Timing::Updateable::close(){
	Timing::deleteTiming(this);
	m_isOpen=false;
}

/*
 * Chronometer methods
 */

inline void Timing::Chronometer::start(){
	m_start=now();
}

inline void Timing::Chronometer::end(){
	m_end=now();
}

inline Timing::time_unit Timing::Chronometer::getElapsed() const{
	return Timing::diff(m_end, m_start);
}

inline const Timing::time_point& Timing::Chronometer::getStart() const{
	return m_start;
}

inline const Timing::time_point& Timing::Chronometer::getEnd() const{
	return m_end;
}

/*
 * Time unit/point operations
 */
inline Timing::time_point Timing::now(){
	return ZUAZO_CLOCK::now();
}

inline Rational Timing::toRat(const time_unit& tu){
	return Rational(tu.count(), TIME_UNITS_PER_SECOND);
}

inline Timing::time_unit Timing::toTu(const Rational& rat){
	if(rat.den)
		return time_unit(rat.num * TIME_UNITS_PER_SECOND / rat.den);
	else
		return time_unit(0);
}

inline Timing::time_unit Timing::diff(const time_point& a, const time_point& b){
	return std::chrono::duration_cast<time_unit>(b - a);
}


} /* namespace Zuazo */
