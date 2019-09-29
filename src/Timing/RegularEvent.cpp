#include <Timing/RegularEvent.h>

#include <Timing/MainThread.h>

namespace Zuazo::Timing {

RegularEvent::~RegularEvent() {
    ZUAZO_EVENT_AUTO_DISABLE
}

void RegularEvent::enable(){
    mainThread->getScheduler().addEvent(*this);
    EventBase::enable();
}

void RegularEvent::disable(){
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