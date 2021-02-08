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

	void 						addEvent(size_t emitterId, EventCallback event);
	void						removeEvent(size_t emitterId);
	void 						process();

private:
	using Event = std::pair<size_t, EventCallback>;

	std::mutex					m_mutex;
	std::vector<Event>			m_pendingEvents;
	std::vector<Event>			m_processingEvents;

};

}