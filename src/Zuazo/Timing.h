#pragma once

#include <chrono>
#include <map>
#include <set>
#include <condition_variable>
#include <thread>

#include "Updateable.h"
#include "Utils/Rational.h"

namespace Zuazo{

#define TIME_NAMESPACE std::chrono
#define TIME_DURATION duration
#define TIME_CLOCK steady_clock
#define TIME_UNIT microseconds
#define TIME_POINT time_point

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

		TimeUnit(const Utils::Rational& rat) : TIME_NAMESPACE::TIME_UNIT(
				rat.den ? rat.num * TIME_UNITS_PER_SECOND / rat.den : 0
		){}

		TimeUnit(const TimeUnit& other) : TIME_NAMESPACE::TIME_UNIT(
				static_cast<TIME_NAMESPACE::TIME_UNIT>(other)
		){}

		TimeUnit(const TIME_NAMESPACE::TIME_UNIT& other) : TIME_NAMESPACE::TIME_UNIT(other){

		}

		~TimeUnit()=default;

		operator Utils::Rational() const{
			return Utils::Rational(count(), TIME_UNITS_PER_SECOND);
		}
		double seconds(){
			return (double)count()/TIME_UNITS_PER_SECOND;
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

	enum class UpdatePriority{
		INPUT,
		CONSUMER
	};

	template <UpdatePriority TPriority>
	class Periodic : Updateable{
		friend Timing;
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

	Timing()=delete;
	Timing(const Timing&)=delete;
	~Timing()=delete;

	static int							init();
	static int 							end();

	static TimePoint					now();

	static void 						addTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval);
	static void 						addTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval);
	static void 						deleteTiming(Periodic<UpdatePriority::INPUT>* event);
	static void 						deleteTiming(Periodic<UpdatePriority::CONSUMER>* event);
	static void 						modifyTiming(Periodic<UpdatePriority::INPUT>* event, const TimeUnit& interval);
	static void 						modifyTiming(Periodic<UpdatePriority::CONSUMER>* event, const TimeUnit& interval);
private:
	struct UpdateInterval{
		std::set<Periodic<UpdatePriority::INPUT>*>	inputs;
		std::set<Periodic<UpdatePriority::CONSUMER>*>	outputs;
		TimeUnit							timeSinceLastUpdate;

		UpdateInterval()=default;
		UpdateInterval(const UpdateInterval& other)=default;
		UpdateInterval(const TimeUnit& interval) : timeSinceLastUpdate(interval){}
		~UpdateInterval()=default;
	};

	static void							addInterval(const TimeUnit& interval);
	static void 						delUnusedInterval();

	static std::map<TimeUnit, UpdateInterval> s_timings;

	static std::thread 					s_thread;
	static bool							s_exit;
	static std::mutex 					s_mutex;
	static std::condition_variable		s_cond;

	static TimePoint					s_currTime;

	static void							updateThreadFunc();
};

/*
 * INLINE METHOD DEFINITIONS
 */

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
 * Periodic methods
 */

template <Timing::UpdatePriority TPriority>
inline Timing::Periodic<TPriority>::Periodic() : Updateable(){

}

template <Timing::UpdatePriority TPriority>
inline Timing::Periodic<TPriority>::Periodic(const Utils::Rational& rate) : Updateable(){
	setRate(rate);
}

template <Timing::UpdatePriority TPriority>
inline Timing::Periodic<TPriority>::Periodic(const Periodic& other) : Updateable(other){
	setInterval(other.m_updateInterval);
}

template <Timing::UpdatePriority TPriority>
inline Timing::Periodic<TPriority>::~Periodic(){
	Timing::deleteTiming(this);
}



template <Timing::UpdatePriority TPriority>
inline const Utils::Rational& Timing::Periodic<TPriority>::getInterval() const {
	return m_updateInterval;
}

template <Timing::UpdatePriority TPriority>
inline Utils::Rational Timing::Periodic<TPriority>::getRate() const {
	return 1/m_updateInterval;
}


template <Timing::UpdatePriority TPriority>
inline void Timing::Periodic<TPriority>::open(){
	Updateable::open();
	Timing::addTiming(this, m_updateInterval);
}

template <Timing::UpdatePriority TPriority>
inline void Timing::Periodic<TPriority>::close(){
	Timing::deleteTiming(this);
	Updateable::close();
}

template <Timing::UpdatePriority TPriority>
inline void Timing::Periodic<TPriority>::setInterval(const Utils::Rational& interval){
	m_updateInterval=interval;

	if(isOpen()){
		Timing::TimeUnit tuInterval(m_updateInterval);
		Timing::modifyTiming(this, tuInterval);
	}
}

template <Timing::UpdatePriority TPriority>
inline void Timing::Periodic<TPriority>::setRate(const Utils::Rational& rate){
	if(rate)
		setInterval(1/rate);
	else
		setInterval(0);
}


/*
 * Other methods
 */
inline Timing::TimePoint Timing::now(){
	return s_currTime;
}
} /* namespace Zuazo */
