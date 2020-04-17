#pragma once

#include <Timing/Chrono.h>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace Zuazo::Timing {

class Scheduler;

class MainLoop {
public:
	MainLoop(Scheduler& scheduler, std::mutex& mutex);
	MainLoop(const MainLoop& other) = delete;
	~MainLoop();

	MainLoop&				operator=(const MainLoop& other) = delete;

	void					interrupt();
private:
	Scheduler& 				m_scheduler;
	std::mutex&				m_mutex;

	bool					m_exit;

	std::thread 			m_thread;
	std::condition_variable m_condition;

	void 					threadFunc();
};

}