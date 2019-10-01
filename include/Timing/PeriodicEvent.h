#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class PeriodicEvent : public EventBase {
public:
    using EventBase::EventBase;
    virtual ~PeriodicEvent();

    void                setPeriod(const Period& period);
    const Period&       getPeriod() const;
    void                setRate(const Rate& rate);
    Rate                getRate() const;

    virtual void        enable() final;
    virtual void        disable() final;
protected:

private:
    Period              m_period = Period::zero();
};

}