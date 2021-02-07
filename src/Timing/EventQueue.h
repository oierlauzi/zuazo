#pragma once

#include <vector>
#include <functional>
#include <mutex>

namespace Zuazo::Timing {

class EventQueue {
public:
	using EventCallback = std::function<void()>;

	EventQueue() = default;
	~EventQueue() = default;

	void 						addEvent(EventCallback event);
	void 						process();

private:
	std::mutex					m_mutex;
	std::vector<EventCallback>	m_pendingEvents;
	std::vector<EventCallback>	m_processingEvents;

};

}