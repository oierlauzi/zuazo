#pragma once


namespace Zuazo::Timing {

class Scheduler;

class EventBase {
    friend Scheduler;
public:
    struct UniqueDisable;

    EventBase() = default;
    EventBase(const EventBase& other) = default;
    EventBase(EventBase&& other) = default;
    virtual ~EventBase() = default;

    EventBase& operator=(const EventBase& other) = default;
    EventBase& operator=(EventBase&& other) = default;

    virtual void        enable();
    virtual void        disable();
    bool                isEnabled() const;

protected:
    virtual void        update() = 0;
    
private:
    bool                m_isEnabled = false;
};
    

class EventBase::UniqueDisable {
public:
    UniqueDisable() = default;
    UniqueDisable(EventBase& evnt);
    UniqueDisable(const UniqueDisable& other) = delete;
    UniqueDisable(UniqueDisable&& other);
    ~UniqueDisable();
private:
    EventBase* m_evnt = nullptr;
};

}

#include "EventBase.inl"