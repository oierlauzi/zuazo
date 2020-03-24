#include <Timing/EventBase.h>

namespace Zuazo::Timing {

void EventBase::enable(){
	m_isEnabled = true;
}

void EventBase::disable(){
	m_isEnabled = false;
}

bool EventBase::isEnabled() const{
	return m_isEnabled;
}

void EventBase::setCallback(UpdateCbk&& cbk){
	m_callback = std::move(cbk);
}

const EventBase::UpdateCbk& EventBase::getCallback() const {
	return m_callback;
}

void EventBase::update() {
	if(m_callback) m_callback();
}


}