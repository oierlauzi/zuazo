#pragma once

#include "EventBase.h"
#include "RegularEvent.h"
#include "PeriodicEvent.h"
#include "Chrono.h"

#include <functional>
#include <map>
#include <set>
#include <queue>

namespace Zuazo {
    class MainThread;
}

namespace Zuazo::Timing {

class Scheduler {
    friend RegularEvent;
    friend PeriodicEvent;
public:
    Scheduler() = default;
    Scheduler(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other) = default;
    ~Scheduler();

    Duration        getTimeForNextEvent() const;
    void            addTime(Duration dura);
private:
    struct PriorityCmp{
        bool operator()(const EventBase* a, const EventBase* b) const;
    };

    template <typename T>
    using EventSet = std::set<T, PriorityCmp>;

    struct PeriodData {
        Duration                    timeForNextUpdate = Duration::zero();
        EventSet<PeriodicEvent*>    evnts;
    };

    using RegularEvents = EventSet<RegularEvent*>;
    using PeriodicEvents = std::map<Period, PeriodData>;


    RegularEvents   m_regularEvnts; 
    PeriodicEvents  m_periodicEvnts;


    void            addEvent(RegularEvent& evnt);
    void            removeEvent(RegularEvent& evnt);
    void            addEvent(PeriodicEvent& evnt);
    void            removeEvent(PeriodicEvent& evnt);
};

inline bool Scheduler::PriorityCmp::operator()(const EventBase* a, const EventBase* b) const {
    if(a->getPriority() < b->getPriority()) return true;
    else if(a->getPriority() > b->getPriority()) return false;
    else return a < b;
};

}