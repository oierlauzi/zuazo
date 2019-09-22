#pragma once

#include "EventBase.h"
#include "RegularEvent.h"
#include "PeriodicEvent.h"
#include "Chrono.h"

#include <set>
#include <map>


namespace Zuazo::Timing {

class Scheduler {
public:
    struct PriorityCmp{
        bool operator()(const EventBase* a, const EventBase* b) const;
    };
    using EventSet = std::set<const EventBase*, PriorityCmp>;

    Scheduler() = default;
    Scheduler(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other) = default;
    ~Scheduler() = default;

    void                            addEvent(const RegularEvent& evnt);
    void                            removeEvent(const RegularEvent& evnt);
    void                            addEvent(const PeriodicEvent& evnt);
    void                            removeEvent(const PeriodicEvent& evnt);

    Duration                        getTimeForNextEvent() const;
    void                            addTime(Duration dura);
private:
    struct PeriodData {
        Duration        timeForNextUpdate;
        EventSet        evnts;
    };

    EventSet                        m_regularEvnts; 
    std::map<Period, PeriodData>    m_periodicEvnts;
};

}