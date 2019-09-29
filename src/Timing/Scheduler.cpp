#include <Timing/Scheduler.h>

#include <typeinfo>

namespace Zuazo::Timing {

Scheduler::~Scheduler(){
    std::set<EventBase*> allEvents;

    //Add all regular events
    allEvents.insert(m_regularEvnts.begin(), m_regularEvnts.end());

    //Add all periodic events
    for(auto perData : m_periodicEvnts){
        allEvents.insert(
            perData.second.evnts.begin(), 
            perData.second.evnts.end()
        );
    }

    //Disable em'
    for(auto evnt : allEvents){
        evnt->disable();
    }
}

void Scheduler::addEvent(RegularEvent& evnt){
    m_regularEvnts.emplace(&evnt);
}

void Scheduler::removeEvent(RegularEvent& evnt){
    m_regularEvnts.erase(&evnt);
}

void Scheduler::addEvent(PeriodicEvent& evnt){
    const Duration& period = evnt.getPeriod();

    if(period > Period::zero()){
        auto ite = m_periodicEvnts.find(period);

        if(ite == m_periodicEvnts.end()){
            //This period does not exist, create it
            std::tie(ite, std::ignore) = m_periodicEvnts.emplace(period, PeriodData());
        }

        //Finally add the event
        ite->second.evnts.emplace(&evnt);
    }
}

void Scheduler::removeEvent(PeriodicEvent& evnt){
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
    if(!m_periodicEvnts.empty() || !m_regularEvnts.empty()){
        //Find the closest update
        Period result(RegularEvent::getMaximumPeriod()); //Calculate minimum, begin with the maximum value

        for(const auto& period : m_periodicEvnts) {
            if(period.second.timeForNextUpdate < result){
                result = period.second.timeForNextUpdate;
            }
        }

        if(result < Duration::zero()) 
            return Duration::zero();
        else
            return result;
    } else {
        return Duration::max(); 
    }
}

void Scheduler::addTime(Duration dura){
    EventSet<EventBase*> pendingEvents;

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
    for(EventBase* event : pendingEvents) {
        event->update();
    }
}
}