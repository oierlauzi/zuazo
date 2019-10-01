#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class RegularEvent : public EventBase {
public:
    using EventBase::EventBase;
    virtual ~RegularEvent();

    virtual void    enable() final;
    virtual void    disable() final;

    static void     setMaximumPeriod(Period period);
    static Period   getMaximumPeriod();
    static void     setMinimumRate(const Rate& rate);
    static Rate     getMinimumRate();
private:
    static Period   s_maximumPeriod;
};

}