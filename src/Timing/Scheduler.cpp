#include <Timing/Scheduler.h>

namespace Zuazo::Timing {

std::unique_ptr<Scheduler> scheduler;

Scheduler::Scheduler() :
    m_exit(false),
    m_thread(this, &Scheduler::loopFunc)
{
}

Scheduler::~Scheduler(){
    m_exit = true;
    m_cond.notify_one();
    m_thread.join();
}

void Scheduler::loopFunc(){
    while(!m_exit){
        //Lock the mutex
        std::unique_lock<std::mutex> lock(m_mutex);

        //Calculete the time for the closest update
        auto timeForNextUpdate = Duration::max();
        for(const auto& period : m_periodicEvnts) {
            if(period.second.timeForNextUpdate < timeForNextUpdate){
                timeForNextUpdate = period.second.timeForNextUpdate;
            }
        }

        //Evaluate whether there is a pending event
        if(timeForNextUpdate == Duration::max()){
            //There is no pending event. Wait for one
            m_cond.wait(lock);

            //Upate the time accordingly
            m_now = Clock::now();
        } else {
            //There is a event nearby
            //Sleep until the update is needed
            m_now += timeForNextUpdate;
            lock.unlock();
            std::this_thread::sleep_until(m_now);
            lock.lock();

            //Get all the pending events
            EventSet events;

            //Insert all the corresponding periodic updates
            for(auto& period : m_periodicEvnts){
                period.second.timeForNextUpdate -= timeForNextUpdate;

                if(period.second.timeForNextUpdate <= Duration::zero()){
                    //This period needs to be updated
                    events.insert(period.second.evnts.cbegin(), period.second.evnts.cend());
                    period.second.timeForNextUpdate += period.first;
                }

            }

            //Insert all the regular updates
            events.insert(m_regularEvnts.cbegin(), m_regularEvnts.cend());

            //Finaly update them all
            for(const EventBase* event : events) {
                event->update();
            }
        }
    }
}

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
        m_cond.notify_one();
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

}