#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class PeriodicEvent : public EventBase {
public:
    PeriodicEvent(Priority prior);
    PeriodicEvent(Priority prior, const Rate& rate);
    PeriodicEvent(Priority prior, const Duration& period);
    PeriodicEvent(const PeriodicEvent& other) = default;
    ~PeriodicEvent();
protected:
    
private:
    Duration        m_period;
};

}