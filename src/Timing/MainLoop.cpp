#include <Timing/MainLoop.h>

namespace Zuazo::Timing {

MainLoop::MainLoop() :
	m_now(Timing::Clock::now()),
	m_exit(false),
	m_thread(&MainLoop::threadFunc, this)
{
}


MainLoop::~MainLoop(){
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_exit.store(true);
		m_handleEvents.notify_all();
	}

	m_thread.join(); //Wait until it finishes
}

TimePoint MainLoop::getCurrentTime() const{
	return m_now;
}

Duration MainLoop::getElapsed() const{
	return m_elapsed;
}

Scheduler& MainLoop::getScheduler(){
	return m_scheduler;
}

const Scheduler& MainLoop::getScheduler() const{
	return m_scheduler;
}

std::mutex& MainLoop::getMutex(){
	return m_mutex;
}

void MainLoop::handleEvents(){
	m_handleEvents.notify_all();
}

void MainLoop::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex); //Adquire the mutex

	while(m_exit.load() == false){
		if( (m_elapsed = m_scheduler.getTimeForNextEvent()) != Timing::Duration::max()) {
			//Threre are scheduled events

			m_now += m_elapsed;
			if(m_handleEvents.wait_until(lock, m_now) == std::cv_status::no_timeout){
				continue; //Re-evaluate
			}

			//Update scheduled events
			m_scheduler.addTime(m_elapsed);
		} else {
			//No events nearby. Sleep.
			m_handleEvents.wait(lock);
		}
	}
}

}