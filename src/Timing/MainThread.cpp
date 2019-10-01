#include <Timing/MainThread.h>

#include <Zuazo.h>

namespace Zuazo::Timing {

std::unique_ptr<MainThread> mainThread;

MainThread::MainThread() :
    m_now(Timing::Clock::now()),
    m_exit(false),
    m_thread(&MainThread::threadFunc, this)
{
    //Wait until everything is set up
    std::unique_lock<std::mutex> lock(m_mutex);
    m_eventsHandled.wait(lock);
}


MainThread::~MainThread(){
    std::unique_lock<std::mutex> lock(m_mutex);
    m_exit = true;
    handleEvents();

    lock.unlock();
    m_thread.join(); //Wait until it finishes
}

void MainThread::threadFunc(){
    std::unique_lock<std::mutex> lock(m_mutex); //Adquire the mutex
    Duration timeForNextEvent;

    //Set up everything
    Zuazo::_setup();
    m_eventsHandled.notify_all(); //Inform the constructor that it can continue

    while(!m_exit){
        //Evaluate what to do
        if(m_execution) {
            m_execution(); //Run it
            m_execution = std::function<void()>(); //Delete it
            m_eventsHandled.notify_all(); //Indicate it has been completed
            m_handleEvents.wait(lock); //Wait for next event
        } else if( (timeForNextEvent = m_scheduler.getTimeForNextEvent()) != Timing::Duration::max()) {
            //Threre are scheduled events
            //Wait until the moment arises
            if(m_handleEvents.wait_until(lock, m_now + timeForNextEvent) == std::cv_status::timeout){
                //Calculate the elapsed time and update
                auto lastUpdate = m_now;
                m_now = Timing::Clock::now();
                m_elapsed = m_now - lastUpdate;

                if(true){ //Try lock contex, to avoid deadlocks. //TODO
                    //Update scheduled events
                    m_scheduler.addTime(m_elapsed);
                } else{
                    m_handleEvents.wait(lock); //Wait until an event is signaled
                }
            } //else another event arised earlier
        } else {
            //No events nearby. Sleep.
            m_handleEvents.wait(lock);
        }
    }

    //Clean everything
    Zuazo::_cleanup();
}

}