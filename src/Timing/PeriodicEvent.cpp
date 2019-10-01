#include <Timing/PeriodicEvent.h>

#include <Timing/MainThread.h>

namespace Zuazo::Timing {

Period RegularEvent::s_maximumPeriod = Period::max();

PeriodicEvent::~PeriodicEvent() {
    ZUAZO_EVENT_AUTO_DISABLE
}

void PeriodicEvent::setPeriod(const Period& period){
    UniqueDisable dis(*this);
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

void PeriodicEvent::enable(){
    std::lock_guard<MainThread> lock(*mainThread);
    mainThread->getScheduler().addEvent(*this);
    EventBase::enable();
}

void PeriodicEvent::disable(){
    std::lock_guard<MainThread> lock(*mainThread);
    mainThread->getScheduler().removeEvent(*this);
    EventBase::disable();
}

}