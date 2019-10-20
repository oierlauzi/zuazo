#pragma once

#include "EventBase.h"
#include "Chrono.h"

#include <functional>
#include <map>
#include <set>

namespace Zuazo::Timing {

class Scheduler {
public:
    using Priority = int32_t;

    class Event : public EventBase {
        friend Scheduler;
    public:
        Event() = default;
        Event(Scheduler* sch, Priority prior);
        Event(const Event& other);
        virtual ~Event();

        Event&              operator=(const Event& other);

        virtual void        enable() final;
        virtual void        disable() final;

        void                setScheduler(Scheduler* sch);
        Scheduler*          getScheduler() const;

        void                setPriority(Priority prior);
        Priority            getPriority() const;

        void                setPeriod(Period period);
        Period              getPeriod() const;
        void                setRate(const Rate& rate);
        Rate                getRate() const;

    private:
        Scheduler*          m_scheduler = nullptr;
        Priority            m_priority = 0;
        Period              m_period = Period::zero();
    };

    friend Event;

    Scheduler() = default;
    Scheduler(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other);
    ~Scheduler();

    Duration                getTimeForNextEvent() const;
    void                    addTime(Duration dura);

private:
    struct PriorityCmp{
        bool operator()(const Event* a, const Event* b) const;
    };

    std::set<Event*, PriorityCmp> m_events;
    std::map<Period, Duration>  m_periods;
};

}