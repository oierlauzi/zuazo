#include <Timing/MainLoop.h>

namespace Zuazo::Timing {

std::unique_ptr<MainLoop> MainLoop::s_mainLoop;

MainLoop::MainLoop() :
	m_now(Timing::Clock::now()),
	m_exit(false),
	m_thread(&MainLoop::threadFunc, this)
{
}


MainLoop::~MainLoop(){
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_exit = true;
		m_handleEvents.notify_all();
	}

	m_thread.join(); //Wait until it finishes
}

void MainLoop::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex); //Adquire the mutex

	while(!m_exit){
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