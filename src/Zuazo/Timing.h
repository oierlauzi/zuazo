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

#define TIME_NAMESPACE std::chrono
#define TIME_DURATION duration
#define TIME_CLOCK steady_clock
#define TIME_UNIT microseconds
#define TIME_POINT time_point

class Updateable;

/*
 * Class containing all the timing information
 */
class Timing{
public:

	/*
	 * Zuazo's time unit
	 */
	class TimeUnit : public TIME_NAMESPACE::TIME_UNIT{
	public:
		using TIME_NAMESPACE::TIME_UNIT::TIME_DURATION; //Use default constructor/destructors

		TimeUnit(const Rational& rat) : TIME_NAMESPACE::TIME_UNIT(
				rat.den ? rat.num * TIME_UNITS_PER_SECOND / rat.den : 0
		){}

		TimeUnit(const TimeUnit& other) : TIME_NAMESPACE::TIME_UNIT(
				static_cast<TIME_NAMESPACE::TIME_UNIT>(other)
		){}

		TimeUnit(const TIME_NAMESPACE::TIME_UNIT& other) : TIME_NAMESPACE::TIME_UNIT(other){

		}

		~TimeUnit()=default;

		operator Rational() const{
			return Rational(count(), TIME_UNITS_PER_SECOND);
		}

		static const int64_t TIME_UNITS_PER_SECOND;
	};

	/*
	 * A class for representing instants of time
	 */
	class TimePoint : public TIME_NAMESPACE::TIME_CLOCK::TIME_POINT{
	public:
		using TIME_NAMESPACE::TIME_CLOCK::TIME_POINT::TIME_POINT; //Use default constructor/destructors

		TimePoint(const TIME_NAMESPACE::TIME_CLOCK::TIME_POINT& other) : TIME_NAMESPACE::TIME_CLOCK::TIME_POINT(other){

		}

		TimePoint(const TimePoint& other) : TIME_NAMESPACE::TIME_CLOCK::TIME_POINT(other){

		}

		~TimePoint()=default;

		TimeUnit operator -(const TimePoint& other) const{
			return TIME_NAMESPACE::duration_cast<TIME_NAMESPACE::TIME_UNIT>(
					static_cast<TIME_NAMESPACE::TIME_CLOCK::TIME_POINT> (*this) - static_cast<TIME_NAMESPACE::TIME_CLOCK::TIME_POINT> (other)
					);
		}

		static TimePoint now(){
			return TIME_NAMESPACE::TIME_CLOCK::now();
		}
	};



	class Chronometer{
	public:
		Chronometer()=default;
		Chronometer(const Chronometer& other)=default;
		~Chronometer()=default;

		void								start();
		void								end();
		TimeUnit							getElapsed() const;
		const TimePoint&					getStart() const;
		const TimePoint&					getEnd() const;

	private:
		TimePoint							m_start;
		TimePoint							m_end;
	};

	Timing()=delete;
	Timing(const Timing&)=delete;
	~Timing()=delete;

	static int							init();
	static int 							end();

	static TimePoint					now();

	static void 						addTiming(Updateable* event, const TimeUnit& interval);
	static void 						deleteTiming(Updateable* event);
	static void 						modifyTiming(Updateable* event, const TimeUnit& interval);
private:
	struct UpdateInterval{
		std::set<Updateable *>				updateables;
		TimeUnit							timeSinceLastUpdate;

		UpdateInterval()=default;
		UpdateInterval(const UpdateInterval& other)=default;
		UpdateInterval(const TimeUnit& interval) : timeSinceLastUpdate(interval){}
		~UpdateInterval()=default;
	};

	static std::map<TimeUnit, UpdateInterval> s_timings;
	static std::thread 					s_thread;
	static bool							s_exit;
	static std::mutex 					s_mutex;
	static std::condition_variable		s_cond;

	static TimePoint					s_currTime;

	static void							updateThreadFunc();
};


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
		virtual void						update()=0;
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

	virtual void						update()=0;
private:
	Rational							m_updateInterval;
	bool								m_isOpen;

	Callback *							m_beforeCbk;
	Callback *							m_afterCbk;

	virtual void						perform();
};

/*
 * INLINE METHOD DEFINITIONS
 */

/*
 *  Timing::Updateable methods
 */

inline const Rational& Updateable::getInterval() const {
	return m_updateInterval;
}

inline Rational Updateable::getRate() const {
	return 1/m_updateInterval;
}

inline bool Updateable::isOpen() const{
	return m_isOpen;
}

inline void	Updateable::perform(){
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if(m_beforeCbk)
		m_beforeCbk->update();

	update();

	if(m_afterCbk)
		m_afterCbk->update();
}

inline void Updateable::open(){
	Timing::addTiming(this, Timing::TimeUnit(getInterval()));
	m_isOpen=true;
}

inline void Updateable::close(){
	Timing::deleteTiming(this);
	m_isOpen=false;
}

/*
 * Chronometer methods
 */

inline void Timing::Chronometer::start(){
	m_start=TimePoint::now();
}

inline void Timing::Chronometer::end(){
	m_end=TimePoint::now();
}

inline Timing::TimeUnit Timing::Chronometer::getElapsed() const{
	return m_end - m_start;
}

inline const Timing::TimePoint& Timing::Chronometer::getStart() const{
	return m_start;
}

inline const Timing::TimePoint& Timing::Chronometer::getEnd() const{
	return m_end;
}

/*
 * Other methods
 */
inline Timing::TimePoint Timing::now(){
	return s_currTime;
}
} /* namespace Zuazo */
