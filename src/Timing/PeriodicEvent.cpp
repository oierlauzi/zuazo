#include <Timing/PeriodicEvent.h>

namespace Zuazo::Timing {

EventBase::EventBase(Priority prior) :
    m_priority(prior)
{
}


void EventBase::setPriority(Priority prior) {
    m_priority = prior;
}

EventBase::Priority EventBase::getPriority() const{
    return m_priority;
}

}