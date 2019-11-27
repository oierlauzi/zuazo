#pragma once

#include "Scheduler.h"
#include "Chrono.h"
#include "../Macros.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

namespace Zuazo::Timing {

class MainLoop {
public:
	MainLoop();
	MainLoop(const MainLoop& other) = delete;
	virtual ~MainLoop();

	TimePoint                           getCurrentTime() const;
	Duration                            getElapsed() const;
	Scheduler&                          getScheduler();
	const Scheduler&                    getScheduler() const;
	std::mutex&                         getMutex();

	void                                handleEvents();
private:
	Scheduler                           m_scheduler;

	TimePoint                           m_now;
	Duration                            m_elapsed;

	std::atomic<bool>                   m_exit;
	mutable std::mutex                  m_mutex;
	mutable std::condition_variable     m_handleEvents;
	std::thread                         m_thread;

	void                                threadFunc();
};

}