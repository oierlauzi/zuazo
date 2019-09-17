#include <Timing/RegularEvent.h>

#include <Timing/Scheduler.h>

namespace Zuazo::Timing {

RegularEvent::RegularEvent(Priority prior) : EventBase(prior)
{
}

void RegularEvent::enable(){
    scheduler->addEvent(*this);
    EventBase::enable();
}

void RegularEvent::disable(){
    scheduler->removeEvent(*this);
    EventBase::disable();
}


}