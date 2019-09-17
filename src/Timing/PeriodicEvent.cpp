#include <Timing/PeriodicEvent.h>

#include <Timing/Scheduler.h>

namespace Zuazo::Timing {

PeriodicEvent::PeriodicEvent(Priority prior) : EventBase(prior),
    m_period(0)
{
}

PeriodicEvent::PeriodicEvent(Priority prior, const Duration& period) : EventBase(prior),
    m_period(period)
{
}

PeriodicEvent::PeriodicEvent(Priority prior, const Rate& rate) : EventBase(prior),
    m_period(Timing::getPeriod(rate))
{
}

void PeriodicEvent::enable(){
    scheduler->addEvent(*this);
    EventBase::enable();
}

void PeriodicEvent::disable(){
    scheduler->removeEvent(*this);
    EventBase::disable();
}

void PeriodicEvent::setPeriod(const Period& period){
    UniqueDisable dis(*this); //Disable temporaly
    m_period = period;
}

const Period& PeriodicEvent::getPeriod() const{
    return m_period;
}

void PeriodicEvent::setRate(const Rate& rate){
    setPeriod(Timing::getPeriod(rate));
}

Rate PeriodicEvent::getRate() const{
    return Timing::getRate(m_period);
}

}