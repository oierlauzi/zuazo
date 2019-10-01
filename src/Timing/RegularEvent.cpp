#include <Timing/RegularEvent.h>

#include <Timing/MainThread.h>

namespace Zuazo::Timing {

RegularEvent::~RegularEvent() {
    ZUAZO_EVENT_AUTO_DISABLE
}

void RegularEvent::enable(){
    std::lock_guard<MainThread> lock(*mainThread);
    mainThread->getScheduler().addEvent(*this);
    EventBase::enable();
}

void RegularEvent::disable(){
    std::lock_guard<MainThread> lock(*mainThread);
    mainThread->getScheduler().removeEvent(*this);
    EventBase::disable();
}

void RegularEvent::setMaximumPeriod(Period period){
    s_maximumPeriod = period;
}

Period RegularEvent::getMaximumPeriod(){
    return s_maximumPeriod;
}

void RegularEvent::setMinimumRate(const Rate& rate){
    setMaximumPeriod(getPeriod(rate));
}

Rate RegularEvent::getMinimumRate(){
    return getRate(getMaximumPeriod());
}

}