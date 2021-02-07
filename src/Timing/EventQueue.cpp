#include "EventQueue.h"

#include <zuazo/Utils/Functions.h>

#include <cassert>

namespace Zuazo::Timing {

void EventQueue::addEvent(EventCallback event) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pendingEvents.push_back(std::move(event));
}

void EventQueue::process() {
	//Populate the processing event list
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		std::swap(m_processingEvents, m_pendingEvents);
		assert(m_pendingEvents.size() == 0);
	}

	//Call all events. In order to avoid deadlocks we'll work on a copy
	for(const auto& event : m_processingEvents) {
		Utils::invokeIf(event);
	}
	m_processingEvents.clear();
}

}