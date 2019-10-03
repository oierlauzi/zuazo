#include <Timing/Scheduler.h>

#include <typeinfo>

namespace Zuazo::Timing {

Scheduler::~Scheduler(){
    EventSet allEvents;

    //Add all the events
    for(auto data : m_events){
        allEvents.insert(
            data.second.events.begin(), 
            data.second.events.end()
        );
    }

    //Disable em'
    for(auto evnt : allEvents){
        evnt->disable();
    }
}

void Scheduler::addEvent(ScheduledEvent& evnt){
    const Duration& period = evnt.getPeriod();
    m_events[period].events.emplace(&evnt); //Creates the new period if non existant
}

void Scheduler::removeEvent(ScheduledEvent& evnt){
    const Duration& period = evnt.getPeriod();
    auto ite = m_events.find(period);

    if(ite != m_events.end()){
        //Period exists, erase the element from it
        ite->second.events.erase(&evnt);

        if(ite->second.events.empty()){
            //No remaining events on this period, erase it
            m_events.erase(ite);
        }
    }
}

Duration Scheduler::getTimeForNextEvent() const{
    if(!m_events.empty()){
        //Find the closest update
        Period result(ScheduledEvent::getDefaultPeriod()); //Calculate minimum, begin with the default value

        for(const auto& period : m_events) {
            if(period.first.count() > 0){ //Evaluate it only for periodic events
                if(period.second.timeForNextUpdate < result){
                    result = period.second.timeForNextUpdate;
                }
            }
        }

        if(result < Duration::zero()) 
            return Duration::zero();
        else
            return result;
    } else {
        return Duration::max(); //Scheduler is empty
    }
}

void Scheduler::addTime(Duration dura){
    EventSet pendingEvents;

    //Insert all the pending updates
    for(auto& period : m_events){
        if(period.first.count() > 0){
            //Insert all the periodic updates
            period.second.timeForNextUpdate -= dura;

            if(period.second.timeForNextUpdate <= Duration::zero()){
                //This period needs to be updated
                pendingEvents.insert(period.second.events.cbegin(), period.second.events.cend());
                period.second.timeForNextUpdate += period.first;
            }
        } else {
            //Insert all the regular updates
            pendingEvents.insert(period.second.events.cbegin(), period.second.events.cend());
        }
    }

    //Finaly update them all
    for(ScheduledEvent* event : pendingEvents) {
        event->update();
    }
}
}