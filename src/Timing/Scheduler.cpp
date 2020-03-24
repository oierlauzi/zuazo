#include <Timing/Scheduler.h>

#include <map>
#include <algorithm>
#include <numeric>

namespace Zuazo::Timing {


/*
 * Scheduler
 */

struct Scheduler::Impl {
	using PeriodMap = std::map<Flicks, EventSet>;

	TimePoint currTime;
	Flicks deltaT;
	EventSet updates;

	mutable EventSet regularEvents;
	mutable PeriodMap periodicEvents;



	void setTime(TimePoint tp) {
		//Update time related variables
		deltaT = tp - currTime;
		currTime = tp;

		//Clear the update set to start over
		updates.clear();

		//Insert the regular updates
		updates.insert(
			updates.cend(), 
			regularEvents.cbegin(), 
			regularEvents.cend()
		);

		//Insert the periodic updates
		for(auto& period : periodicEvents){
			if(currTime.time_since_epoch() % period.first == Flicks::zero()){
				//This period needs to be updated
				updates.insert(
					updates.cend(),
					period.second.cbegin(), 
					period.second.cend()
				);
			}
		}

		//Sort the events from higher priorities to lower ones
		std::sort(
			updates.begin(), 
			updates.end(), 
			[](const ScheduledEvent& a, const ScheduledEvent& b) -> bool {
				return a.getPriority() > b.getPriority();
			}
		);

		//Update all
		for(ScheduledEvent& event : updates){
			event.update();
		}
	}

	TimePoint getTime() const {
		return currTime;
	}

	Flicks getDeltaT() const {
		return deltaT;
	}

	Flicks getTimeForNextEvent() const {
		//Calculate the minumum remaining time
		auto result = Flicks::max();

		for(const auto& period : periodicEvents) {
			const TimePoint nextUpdate((currTime.time_since_epoch() / period.first + 1) * period.first);
			const auto remaining = nextUpdate - currTime;

			if(remaining < result){
				result = remaining;
			}
		}

		return result;
	}

	Flicks getTimeForPrevEvent() const {
		//Calculate the minumum remaining time
		auto result = Flicks::max();

		for(const auto& period : periodicEvents) {
			const TimePoint prevUpdate((currTime.time_since_epoch() / period.first - 1) * period.first);
			const auto remaining = currTime - prevUpdate;

			if(remaining < result){
				result = remaining;
			}
		}

		return result;
	}


	void addEvent(RegularEvent& event) const {
		regularEvents.emplace_back(event);
	}

	void removeEvent(RegularEvent& event) const {
		auto end = std::remove_if(
			regularEvents.begin(), 
			regularEvents.end(), 
			[&] (const ScheduledEvent& e) -> bool {
				return &e == &event;
			}
		);
		regularEvents.erase(end, regularEvents.end());
	}

	void addEvent(PeriodicEvent& event) const {
		const auto period = event.getPeriod();
		auto ite = periodicEvents.find(period);

		if(ite == periodicEvents.cend()){
			//Period does not exist. Create it.

			periodicEvents.emplace(	period, 
									EventSet{event});			
		} else {
			ite->second.emplace_back(event);
		}
	}

	void removeEvent(PeriodicEvent& event) const {
		const auto period = event.getPeriod();
		auto ite = periodicEvents.find(period);

		if(ite != periodicEvents.cend()){
			//Erase it
			auto end = std::remove_if(
				ite->second.begin(), 
				ite->second.end(), 
				[&] (const ScheduledEvent& e) -> bool {
					return &e == &event;
				}
			);
			ite->second.erase(end, ite->second.end());

			if(ite->second.size() == 0){
				//No more events of this period
				periodicEvents.erase(ite);
			}
		}
	}

};



Scheduler::Scheduler() = default;

Scheduler::Scheduler(Scheduler&& other) = default;

Scheduler::~Scheduler() = default;

Scheduler& Scheduler::operator=(Scheduler&& other) = default;



void Scheduler::setTime(TimePoint tp) {
	m_impl->setTime(tp);
}

TimePoint Scheduler::getTime() const {
	return m_impl->getTime();
}

Flicks Scheduler::getDeltaT() const {
	return m_impl->getDeltaT();
}

Flicks Scheduler::getTimeForNextEvent() const {
	return m_impl->getTimeForNextEvent();
}

Flicks Scheduler::getTimeForPrevEvent() const {
	return m_impl->getTimeForPrevEvent();
}


/*
 * UniqueDisable
 */

class UniqueDisable {
public:
	UniqueDisable(EventBase& evnt)
		: m_evnt(evnt.isEnabled() ? &evnt : nullptr)
	{
		if(m_evnt) m_evnt->disable();
	}
	~UniqueDisable() {
		if(m_evnt){
			if(m_evnt) m_evnt->enable();
		}
	}
private:
	EventBase* m_evnt;
};

/*
 * ScheduledEvent
 */

const ScheduledEvent::Priority ScheduledEvent::OUTPUT_PRIORITY = ScheduledEvent::LOWEST_PRIORITY / 2;
const ScheduledEvent::Priority ScheduledEvent::PROCESSOR_PRIORITY = 0;
const ScheduledEvent::Priority ScheduledEvent::INPUT_PRIORITY = ScheduledEvent::HIGHEST_PRIORITY / 2;

ScheduledEvent::ScheduledEvent(const Scheduler& scheduler, Priority priority) 
	: m_scheduler(scheduler)
	, m_priority(priority)
{
}

ScheduledEvent::~ScheduledEvent() = default;

void ScheduledEvent::setScheduler(const Scheduler& scheduler) {
	UniqueDisable dis(*this);
	m_scheduler = scheduler;
}

const Scheduler& ScheduledEvent::getScheduler() const {
	return m_scheduler;
}

void ScheduledEvent::setPriority(Priority priority) {
	UniqueDisable dis(*this);
	m_priority = priority;
}

ScheduledEvent::Priority ScheduledEvent::getPriority() const {
	return m_priority;
}


/*
 * RegularEvent
 */


RegularEvent::~RegularEvent() {
	if(isEnabled()) disable();
}

void RegularEvent::enable() {
	getScheduler().m_impl->addEvent(*this);
	ScheduledEvent::enable();
}

void RegularEvent::disable() {
	getScheduler().m_impl->removeEvent(*this);
	ScheduledEvent::disable();
}

/*
 * PeriodicEvent
 */

PeriodicEvent::~PeriodicEvent() {
	if(isEnabled()) disable();
}

void PeriodicEvent::enable() {
	getScheduler().m_impl->addEvent(*this);
	ScheduledEvent::enable();
}

void PeriodicEvent::disable() {
	getScheduler().m_impl->removeEvent(*this);
	ScheduledEvent::disable();
}

void PeriodicEvent::setPeriod(Flicks period) {
	UniqueDisable dis(*this);
	m_period = period;
}

Flicks PeriodicEvent::getPeriod() const {
	return m_period;
}

void PeriodicEvent::setRate(const Rate& rate) {
	setPeriod(Timing::getPeriod(rate));
}

Rate PeriodicEvent::getRate() const {
	return Timing::getRate(m_period);
}

}