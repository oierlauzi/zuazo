#include <Timing/Scheduler.h>

#include <typeinfo>

namespace Zuazo::Timing {

bool Scheduler::PriorityCmp::operator()(const EventBase* a, const EventBase* b) const {
    if(a->getPriority() < b->getPriority()) return true;
    else if(a->getPriority() > b->getPriority()) return false;
    else return a < b;
};

void Scheduler::addEvent(const RegularEvent& evnt){
    m_regularEvnts.emplace(&evnt);
}

void Scheduler::removeEvent(const RegularEvent& evnt){
    m_regularEvnts.erase(&evnt);
}

void Scheduler::addEvent(const PeriodicEvent& evnt){
    const Duration& period = evnt.getPeriod();

    if(period > Period::zero()){
        auto ite = m_periodicEvnts.find(period);

        if(ite == m_periodicEvnts.end()){
            //This period does not exist, create it
            const PeriodData newPeriod{
                .timeForNextUpdate = Duration::zero(), //Update it ASAP
                .evnts = EventSet()
            };

            //Insert the new period
            std::tie(ite, std::ignore) = m_periodicEvnts.emplace(period, newPeriod);
        }

        //Finally add the event
        ite->second.evnts.emplace(&evnt);
    }
}

void Scheduler::removeEvent(const PeriodicEvent& evnt){
    const Duration& period = evnt.getPeriod();
    auto ite = m_periodicEvnts.find(period);

    if(ite != m_periodicEvnts.end()){
        //Period exists, erase the element from it
        ite->second.evnts.erase(&evnt);

        if(ite->second.evnts.empty()){
            //No remaingin events on this period, erase it
            m_periodicEvnts.erase(ite);
        }
    }
}

Duration Scheduler::getTimeForNextEvent() const{
    //Find the closest update
    Duration result(Duration::max());

    for(const auto& period : m_periodicEvnts) {
        if(period.second.timeForNextUpdate < result){
            result = period.second.timeForNextUpdate;
        }
    }

    return result;
}

void Scheduler::addTime(Duration dura){
    EventSet pendingEvents;

    //Insert all the pending periodic updates
    for(auto& period : m_periodicEvnts){
        period.second.timeForNextUpdate -= dura;

        if(period.second.timeForNextUpdate <= Duration::zero()){
            //This period needs to be updated
            pendingEvents.insert(period.second.evnts.cbegin(), period.second.evnts.cend());
            period.second.timeForNextUpdate += period.first;
        }
    }

    //Insert all the regular updates
    pendingEvents.insert(m_regularEvnts.cbegin(), m_regularEvnts.cend());

    //Finaly update them all
    for(const EventBase* event : pendingEvents) {
        event->update();
    }
}
}