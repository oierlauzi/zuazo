#include <Timing/RegularEvent.h>

namespace Zuazo::Timing {

RegularEvent::RegularEvent(Priority prior) : EventBase(prior)
{
}

RegularEvent::RegularEvent(const RegularEvent& other) : EventBase(other)
{
}

void RegularEvent::enable(){
    //TODO
}

void RegularEvent::disable(){
    //TODO
}

}