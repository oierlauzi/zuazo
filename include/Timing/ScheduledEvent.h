#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class ScheduledEvent : public EventBase {
public:
    using Priority = int;

    ScheduledEvent(Priority prior);
    ScheduledEvent(const ScheduledEvent& other);
    virtual ~ScheduledEvent();

    void                setPriority(Priority prior);
    Priority            getPriority() const;

    void                setPeriod(const Period& period);
    const Period&       getPeriod() const;
    void                setRate(const Rate& rate);
    Rate                getRate() const;

    virtual void        enable() final;
    virtual void        disable() final;

    static void         setDefaultPeriod(Period period);
    static Period       getDefaultPeriod();
    static void         setDefaultRate(const Rate& rate);
    static Rate         getDefaultRate();
private:
    Priority            m_priority;
    Period              m_period = Period::zero();

    static Period       s_defaultPeriod;
};

}