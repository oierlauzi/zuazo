#include <Timing/EventBase.h>

#include <Timing/Scheduler.h>

namespace Zuazo::Timing {

EventBase::EventBase() :
    m_priority(IGNORE),
    m_isEnabled(false)
{
}

EventBase::EventBase(Priority prior) :
    m_priority(prior),
    m_isEnabled(false)
{
}

EventBase::EventBase(const EventBase& other) :
    m_priority(other.m_priority),
    m_isEnabled(false)
{
}

EventBase::~EventBase(){
    ZUAZO_EVENT_AUTO_DISABLE //Just in case
}

void EventBase::setPriority(Priority prior) {
    UniqueDisable dis(*this); //Disable temporaly
    m_priority = prior;
}

EventBase::Priority EventBase::getPriority() const {
    return m_priority;
}

void EventBase::enable(){
    m_isEnabled = true;
}

void EventBase::disable(){
    m_isEnabled = false;
}

bool EventBase::isEnabled() const{
    return m_isEnabled;
}

}