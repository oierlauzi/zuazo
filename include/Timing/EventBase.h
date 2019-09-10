#pragma once

#include "../Utils/PhantomClass.h"

namespace Zuazo::Timing {

class EventBase {
public:
    using Priority = Utils::PhantomClass<int, struct _evntprior>;

    EventBase(Priority prior);
    EventBase(const EventBase& other) = default;
    ~EventBase() = default;
protected:
    void                setPriority(Priority prior);
    Priority            getPriority() const;

    virtual void        update() const = 0;
private:
    Priority            m_priority;
};

}