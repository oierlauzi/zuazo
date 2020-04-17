#include "MainLoop.h"

#include "Scheduler.h"

namespace Zuazo::Timing {

MainLoop::MainLoop(Scheduler& scheduler, std::mutex& mutex)
	: m_scheduler(scheduler)
	, m_mutex(mutex)
	, m_exit(false)
	, m_thread(&MainLoop::threadFunc, this)
{
}

MainLoop::~MainLoop() {
	//Indicate the main thread to finish
	std::unique_lock<std::mutex> lock(m_mutex);
	m_exit = true;
	interrupt();
	lock.unlock();

	//Wait for it to return
	m_thread.join();
}

void MainLoop::interrupt() {
	m_condition.notify_all();
}

void MainLoop::threadFunc() {
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit) {
		const auto remaining = m_scheduler.getTimeForNextEvent();
		if(remaining == Duration::max()) {
			//No events ahead
			m_condition.wait(lock);

			//Update the time after sleeping for that much
			const auto newTime = now();
			m_scheduler.setEpoch(newTime);
			m_scheduler.gotoTime(newTime); //Will update everything
		} else {
			const auto newTime = m_scheduler.getTime() + remaining;

			//Wait until the moment arises
			if(m_condition.wait_until(lock, newTime) == std::cv_status::timeout) {
				//Waiting was not interrupted. Update the scheduler
				m_scheduler.gotoTime(newTime);
			}
		}
	}
}

}