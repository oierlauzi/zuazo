#include <Timing/ScheduledEvent.h>

#include <Timing/MainThread.h>

namespace Zuazo::Timing {

Period ScheduledEvent::s_defaultPeriod = Period::max();




ScheduledEvent::ScheduledEvent(Priority prior) :
    m_priority(prior)
{
}

ScheduledEvent::ScheduledEvent(const ScheduledEvent& other) :
    m_priority(other.m_priority)
{
    if(other.isEnabled()) enable();
}

ScheduledEvent::~ScheduledEvent() {
    ZUAZO_EVENT_AUTO_DISABLE
}




void ScheduledEvent::setPriority(Priority prior){
    UniqueDisable dis(*this);
    m_priority = prior;
}

ScheduledEvent::Priority ScheduledEvent::getPriority() const{
    return m_priority;
}

void ScheduledEvent::setPeriod(const Period& period){
    UniqueDisable dis(*this);
    m_period = period;
}

const Period& ScheduledEvent::getPeriod() const{
    return m_period;
}

void ScheduledEvent::setRate(const Rate& rate){
    setPeriod(Timing::getPeriod(rate));
}

Rate ScheduledEvent::getRate() const{
    return Timing::getRate(m_period);
}




void ScheduledEvent::enable(){
    auto& mainThread = getMainThread();

    std::lock_guard<std::mutex> lock(mainThread.m_mutex);
    mainThread.m_scheduler.addEvent(*this);

    EventBase::enable();
}

void ScheduledEvent::disable(){
    auto& mainThread = getMainThread();

    std::lock_guard<std::mutex> lock(mainThread.m_mutex);
    mainThread.m_scheduler.removeEvent(*this);
    
    EventBase::disable();
}




void ScheduledEvent::setDefaultPeriod(Period period){
    s_defaultPeriod = period;
}

Period ScheduledEvent::getDefaultPeriod(){
    return s_defaultPeriod;
}

void ScheduledEvent::setDefaultRate(const Rate& rate){
    setDefaultPeriod(Timing::getPeriod(rate));
}

Rate ScheduledEvent::getDefaultRate(){
    return Timing::getRate(getDefaultPeriod());
}

}