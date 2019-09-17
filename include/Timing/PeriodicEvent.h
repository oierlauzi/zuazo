#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class PeriodicEvent : public EventBase {
public:
    PeriodicEvent(Priority prior);
    PeriodicEvent(Priority prior, const Rate& rate);
    PeriodicEvent(Priority prior, const Period& period);
    PeriodicEvent(const PeriodicEvent& other) = delete;
    ~PeriodicEvent() = default;

    virtual void        enable() override;
    virtual void        disable() override;

    void                setPeriod(const Period& period);
    const Period&       getPeriod() const;
    void                setRate(const Rate& rate);
    Rate                getRate() const;
protected:

private:
    Period              m_period;
};

}