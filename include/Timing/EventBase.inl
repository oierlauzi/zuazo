#include "EventBase.h"


namespace Zuazo::Timing {

inline EventBase::~EventBase(){
    ZUAZO_EVENT_AUTO_DISABLE //Just in case
}




inline void EventBase::enable(){
    m_isEnabled = true;
}

inline void EventBase::disable(){
    m_isEnabled = false;
}

inline bool EventBase::isEnabled() const{
    return m_isEnabled;
}




inline EventBase::UniqueDisable::UniqueDisable(EventBase& evnt) :
    m_evnt(evnt.isEnabled() ? &evnt : nullptr)
{
    if(m_evnt) m_evnt->disable();
}

inline EventBase::UniqueDisable::UniqueDisable(UniqueDisable&& other) :
    m_evnt(other.m_evnt)
{
    other.m_evnt = nullptr;
}

inline EventBase::UniqueDisable::~UniqueDisable(){
    if(m_evnt) m_evnt->enable();
}

}