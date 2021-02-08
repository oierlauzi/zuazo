#include "EventQueue.h"

#include <zuazo/Utils/Functions.h>

#include <cassert>
#include <algorithm>

namespace Zuazo::Timing {

void EventQueue::addEvent(size_t emitterId, EventCallback event) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pendingEvents.emplace_back(emitterId, std::move(event));
}

void EventQueue::removeEvent(size_t emitterId) {
	std::lock_guard<std::mutex> lock(m_mutex);

	const auto ite = std::remove_if(
		m_pendingEvents.begin(), m_pendingEvents.end(),
		[emitterId] (const auto& event) -> bool {
			return event.first == emitterId;
		}
	);

	m_pendingEvents.erase(ite, m_pendingEvents.cend());
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
		Utils::invokeIf(event.second);
	}
	m_processingEvents.clear();
}

}