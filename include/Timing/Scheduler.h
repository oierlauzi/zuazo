#pragma once

#include "EventBase.h"
#include "ScheduledEvent.h"
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
public:
    Scheduler() = default;
    Scheduler(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other) = default;
    ~Scheduler();

    void            addEvent(ScheduledEvent& evnt);
    void            removeEvent(ScheduledEvent& evnt);

    Duration        getTimeForNextEvent() const;
    void            addTime(Duration dura);
private:
    struct PriorityCmp{
        bool operator()(const ScheduledEvent* a, const ScheduledEvent* b) const;
    };

    using EventSet = std::set<ScheduledEvent*, PriorityCmp>;

    struct PeriodData {
        Duration                    timeForNextUpdate = Duration::zero();
        EventSet                    events;
    };

    std::map<Period, PeriodData> m_events;
};

inline bool Scheduler::PriorityCmp::operator()(const ScheduledEvent* a, const ScheduledEvent* b) const {
    if(a->getPriority() < b->getPriority()) return true;
    else if(a->getPriority() > b->getPriority()) return false;
    else return a < b;
};

}