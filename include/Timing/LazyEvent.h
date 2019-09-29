#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class LazyEvent : public EventBase {
public:
    using EventBase::EventBase;

    void                setMaximumRecursion(uint rec);
    uint                getMaximumRecursion() const;

    virtual void        enable() override;
    virtual void        disable() override;

    void                evaluate();
private:
    uint                m_maxRecursion = 1;
    uint                m_recursion = 0;
};

}