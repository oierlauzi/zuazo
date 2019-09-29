#pragma once

#include <limits>

namespace Zuazo::Timing {

class Scheduler;

class EventBase {
    friend Scheduler;
public:
    using Priority = int;
    struct UniqueDisable;

    EventBase() = default;
    EventBase(Priority prior);
    EventBase(const EventBase& other) = delete;
    virtual ~EventBase();

    void                setPriority(Priority prior);
    Priority            getPriority() const;

    virtual void        enable();
    virtual void        disable();
    bool                isEnabled() const;
protected:
    virtual void        update() = 0;

    static constexpr Priority IGNORE_PRIOR = 0;
    static constexpr Priority MAX_PRIOR = std::numeric_limits<Priority>::min();
    static constexpr Priority MIN_PRIOR = std::numeric_limits<Priority>::max();
private:
    Priority            m_priority = IGNORE_PRIOR;
    bool                m_isEnabled = false;
};

class EventBase::UniqueDisable {
public:
    UniqueDisable() : m_evnt(nullptr) { }
    UniqueDisable(EventBase& evnt) : m_evnt(evnt.isEnabled() ? & evnt : nullptr) { if(m_evnt) m_evnt->disable(); }
    UniqueDisable(const UniqueDisable& other) = delete;
    UniqueDisable(UniqueDisable&& other) : m_evnt(other.m_evnt) { other.m_evnt = nullptr; }
    ~UniqueDisable() { if(m_evnt) m_evnt->enable(); }
private:
    EventBase* m_evnt;
};


#define ZUAZO_EVENT_AUTO_DISABLE \
    if(isEnabled()) disable();

}