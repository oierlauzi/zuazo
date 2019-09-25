#include <Timing/PeriodicEvent.h>

#include <Timing/Loop.h>

namespace Zuazo::Timing {

PeriodicEvent::PeriodicEvent(Priority prior) : EventBase(prior),
    m_period(0)
{
}

void PeriodicEvent::enable(){
    mainLoop->addEvent(*this);
    EventBase::enable();
}

void PeriodicEvent::disable(){
    mainLoop->removeEvent(*this);
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