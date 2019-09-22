#include <Timing/RegularEvent.h>

#include <Timing/Loop.h>

namespace Zuazo::Timing {

RegularEvent::RegularEvent(Priority prior) : EventBase(prior)
{
}

void RegularEvent::enable(){
    mainLoop->addEvent(*this);
    EventBase::enable();
}

void RegularEvent::disable(){
    mainLoop->removeEvent(*this);
    EventBase::disable();
}


}