#pragma once

#include "../Utils/Pimpl.h"
#include "EventBase.h"
#include "Chrono.h"

#include <limits>
#include <functional>
#include <utility>
#include <vector>

namespace Zuazo::Timing {

class ScheduledEvent;
class RegularEvent;
class PeriodicEvent;


class Scheduler {
	friend ScheduledEvent;
	friend RegularEvent; 
	friend PeriodicEvent;

public:
	using EventSet = std::vector<std::reference_wrapper<ScheduledEvent>>;

	Scheduler();
	Scheduler(const Scheduler& other) = delete;
	Scheduler(Scheduler&& other);
	~Scheduler();

	Scheduler&				operator=(const Scheduler& other) = delete;
	Scheduler&				operator=(Scheduler&& other);

	void					setTime(TimePoint tp);
	TimePoint				getTime() const;
	Flicks					getDeltaT() const;

	Flicks               	getTimeForNextEvent() const;
	Flicks               	getTimeForPrevEvent() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;
	
};


class ScheduledEvent
	: public Timing::EventBase 
{
	friend Scheduler::Impl;
public:
	/**
	 * Priority defines in which order Events will be updated, high priority meaning that
	 * it will be updated early, whilst low priority means that it will be updated late.
	 */

	using Priority = uint32_t;
	static constexpr Priority LOWEST_PRIORITY = std::numeric_limits<Priority>::min();
	static constexpr Priority HIGHEST_PRIORITY = std::numeric_limits<Priority>::max();
	static const Priority OUTPUT_PRIORITY;
	static const Priority PROCESSOR_PRIORITY;
	static const Priority INPUT_PRIORITY;

	ScheduledEvent(const Scheduler& scheduler, Priority priority);
	ScheduledEvent(const ScheduledEvent& other) = delete;
	virtual ~ScheduledEvent();

	ScheduledEvent& 		operator=(const ScheduledEvent& other) = delete;

	void					setScheduler(const Scheduler& scheduler);
	const Scheduler&		getScheduler() const;

	void					setPriority(Priority priority);
	Priority				getPriority() const;

private:
	using SchedulerReference = std::reference_wrapper<const Scheduler>;
	using EventBase::update;

	SchedulerReference		m_scheduler;
	Priority				m_priority;

};


class RegularEvent : public ScheduledEvent {
public:
	using ScheduledEvent::ScheduledEvent;
	RegularEvent(const RegularEvent& other) = delete;
	virtual ~RegularEvent();

	RegularEvent&		operator=(const RegularEvent& other);

	virtual void        enable() final;
	virtual void        disable() final;

};


class PeriodicEvent : public ScheduledEvent {
public:
	using ScheduledEvent::ScheduledEvent;
	PeriodicEvent(const PeriodicEvent& other) = delete;
	virtual ~PeriodicEvent();

	PeriodicEvent&		operator=(const PeriodicEvent& other);

	virtual void        enable() final;
	virtual void        disable() final;

	void                setPeriod(Flicks period);
	Flicks              getPeriod() const;
	void                setRate(const Rate& rate);
	Rate                getRate() const;

private:
	Flicks              m_period = Flicks::zero();
	
};


}