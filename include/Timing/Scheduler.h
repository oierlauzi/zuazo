#pragma once

#include "EventBase.h"
#include "RegularEvent.h"
#include "PeriodicEvent.h"
#include "Chrono.h"

#include <set>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>


namespace Zuazo::Timing {

class Scheduler {
    friend RegularEvent;
    friend PeriodicEvent;
public:
    Scheduler();
    Scheduler(const Scheduler& other) = delete;
    ~Scheduler();
private:
    struct PriorityCmp {
        bool operator()(const EventBase* a, const EventBase* b) const;
    };

    using EventSet = std::set<const EventBase*, PriorityCmp>;

    struct PeriodData {
        Duration        timeForNextUpdate;
        EventSet        evnts;
    };

    bool                            m_exit;
    std::thread                     m_thread;
    std::mutex                      m_mutex;
    std::condition_variable         m_cond;

    TimePoint                       m_now;

    EventSet                        m_regularEvnts; 
    std::map<Period, PeriodData>    m_periodicEvnts;

    void                            loopFunc();

    void                            addEvent(const RegularEvent& evnt);
    void                            removeEvent(const RegularEvent& evnt);
    void                            addEvent(const PeriodicEvent& evnt);
    void                            removeEvent(const PeriodicEvent& evnt);
};

extern std::unique_ptr<Scheduler> scheduler;

}