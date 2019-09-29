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

    //Set up everything
    Zuazo::_setup();
    m_eventsHandled.notify_all(); //Inform the constructor that it can continue

    while(!m_exit){
        //Calculate time for next periodic event
        Timing::Duration timeForNextEvent = m_scheduler.getTimeForNextEvent();

        //Wait until something happens
        if(timeForNextEvent == Timing::Duration::max()){
            m_handleEvents.wait(lock); //No events nearby
        }else{
            //Sleep until the moment arrives
            m_handleEvents.wait_until(lock, m_now + timeForNextEvent);
        }

        //Evaluate what to do
        if(m_executions.empty()) {
            //Update scheduled events
            //Calculate the elapsed time and update
            auto lastUpdate = m_now;
            m_now = Timing::Clock::now();
            m_elapsed = m_now - lastUpdate;

            m_scheduler.addTime(m_elapsed);
        } else {
            //Threre are pending executions
            while(!m_executions.empty()){
                m_executions.front()(); //Execute it
                m_executions.pop();
            }

            //Indicate pending executions have been executed
            m_eventsHandled.notify_all();
        }
    }

    //Clean everything
    Zuazo::_cleanup();
}

}