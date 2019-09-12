#pragma once



namespace Zuazo::Timing {

class Scheduler;

class EventBase {
    friend Scheduler;
public:
    using Priority = int;
    struct UniqueDisable;

    EventBase(Priority prior);
    EventBase(const EventBase& other);
    ~EventBase();

    void                setPriority(Priority prior);
    Priority            getPriority() const;

    virtual void        enable();
    virtual void        disable();
    bool                isEnabled() const;
protected:
    virtual void        update() const = 0;
private:
    Priority            m_priority;
    bool                m_isEnabled;
};

struct EventBase::UniqueDisable{
public:
    UniqueDisable(EventBase& evnt) : m_evnt( evnt.isEnabled() ? &evnt : nullptr) {
        if(m_evnt) m_evnt->disable();
    }
    UniqueDisable(const UniqueDisable& other) = delete;
    ~UniqueDisable(){ 
        if(m_evnt) m_evnt->enable();
    }
private:
    EventBase* m_evnt;
}

}