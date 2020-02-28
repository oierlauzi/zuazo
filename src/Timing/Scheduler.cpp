#include <Timing/Scheduler.h>

#include <typeinfo>

namespace Zuazo::Timing {

Scheduler::Scheduler(Scheduler&& other) :
	m_events(std::move(other.m_events)),
	m_periods(std::move(other.m_periods))
{
	for(auto evnt : m_events){
		evnt->m_scheduler = this;
	}
}

Scheduler::~Scheduler(){
	for(auto evnt : m_events){
		evnt->m_scheduler = nullptr;
	}
}

Duration Scheduler::getTimeForNextEvent() const{
	//Find the closest update
	Period result(Duration::max()); //Calculate minimum, begin with the maximum value

	for(const auto& period : m_periods) {
		if(period.first.count() > 0) {
			if(period.second < result){
				result = period.second;
			}
		}
	}

	return result;
}

void Scheduler::addTime(Duration dura){
	for(auto& period : m_periods) {
		if(period.first.count() > 0) {
			period.second -= dura;
		}
	}

	for(auto evnt : m_events){
		auto period = evnt->getPeriod();
		if(period.count() > 0){
			if(m_periods[period].count() <= 0){
				evnt->update();
			}
		} else {
			evnt->update();
		}
	}


	for(auto& period : m_periods) {
		if(period.first.count() > 0) {
			if(period.second.count() <= 0){
				period.second += period.first;
			}
		}
	}

}




Scheduler::Event::Event(Scheduler* sch, Priority prior) :
	m_scheduler(sch),
	m_priority(prior)
{
}

Scheduler::Event::Event(const Event& other) :
	m_scheduler(other.m_scheduler),
	m_priority(other.m_priority),
	m_period(other.m_period)
{
	if(other.isEnabled()) enable();
}

Scheduler::Event::~Event() {
	if(isEnabled()) disable();
}

Scheduler::Event& Scheduler::Event::operator=(const Event& other) {
	if(isEnabled()) disable();

	m_scheduler=other.m_scheduler;
	m_priority=other.m_priority;
	m_period=other.m_period;

	if(other.isEnabled()) enable();

	return *this;
}

void Scheduler::Event::enable(){
	if(m_scheduler){
		m_scheduler->m_events.insert(this);

		auto ite = m_scheduler->m_periods.find(m_period);
		if(ite == m_scheduler->m_periods.cend()){
			m_scheduler->m_periods.emplace(m_period, 0);
		}
	}

	EventBase::enable();
}

void Scheduler::Event::disable(){
	if(m_scheduler){
		m_scheduler->m_events.erase(this);

		bool found = false;
		for(auto evnt : m_scheduler->m_events){
			if(evnt->m_period == m_period){
				found = true;
				break;
			}
		}

		if(!found){
			m_scheduler->m_periods.erase(m_period);
		}
	}
	
	EventBase::disable();
}

void Scheduler::Event::setScheduler(Scheduler* sch){
	EventBase::UniqueDisable dis;
	m_scheduler = sch;
}

Scheduler* Scheduler::Event::getScheduler() const{
	return m_scheduler;
}


void Scheduler::Event::setPriority(Priority prior){
	EventBase::UniqueDisable dis;
	m_priority = prior;
}

Scheduler::Priority Scheduler::Event::getPriority() const{
	return m_priority;
}

void Scheduler::Event::setPeriod(Period period){
	EventBase::UniqueDisable dis;
	m_period = period;
}

Period Scheduler::Event::getPeriod() const {
	return m_period;
}

void Scheduler::Event::setRate(const Rate& rat){
	EventBase::UniqueDisable dis;
	m_period = Timing::getPeriod(rat);
}

Rate Scheduler::Event::getRate() const {
	return Timing::getRate(m_period);
}




bool Scheduler::PriorityCmp::operator()(const Event* a, const Event* b) const {
	if(a->getPriority() < b->getPriority()) return true;
	else if(a->getPriority() > b->getPriority()) return false;
	else return a < b;
}

}