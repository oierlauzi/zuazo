#pragma once

#include <Timing/EventBase.h>
#include <Timing/Chrono.h>

#include <limits>
#include <functional>
#include <utility>
#include <vector>
#include <map>

namespace Zuazo::Timing {

class Scheduler {
public:
	using Priority = int32_t;

	Scheduler() = default;
	Scheduler(const Scheduler& other) = delete;
	Scheduler(Scheduler&& other) = default;
	~Scheduler() = default;

	Scheduler&				operator=(const Scheduler& other) = delete;
	Scheduler&				operator=(Scheduler&& other) = default;

	void					setEpoch(TimePoint tp);
	TimePoint				getEpoch() const;

	void					gotoTime(TimePoint tp);
	TimePoint				getTime() const;
	Duration				getDeltaT() const;

	Duration               	getTimeForNextEvent() const;

	void					addRegularEvent(const EventBase& event, Priority prior);
	void					removeRegularEvent(const EventBase& event);

	void					addPeriodicEvent(const EventBase& event, Priority prior, Duration period);
	void					removePeriodicEvent(const EventBase& event);

private:
	using EventBaseRef = std::reference_wrapper<const EventBase>;
	using Event = std::tuple<EventBaseRef, Priority>;
	using EventSet = std::vector<Event>;
	using PeriodMap = std::map<Duration, EventSet>;

	TimePoint				m_epoch;
	TimePoint 				m_currTime;
	Duration 				m_deltaT;
	EventSet 				m_updates;

	EventSet 				m_regularEvents;
	PeriodMap 				m_periodicEvents;
	
};

}