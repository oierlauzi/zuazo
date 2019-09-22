#include <Timing/Loop.h>

namespace Zuazo::Timing {

std::unique_ptr<Loop> mainLoop;

Loop::Loop() :
    m_now(Clock::now()),
    m_exit(false),
    m_loop(&Loop::loopFunc, this)
{
}

Loop::~Loop(){
    std::unique_lock<std::mutex> lock(m_mutex);
    m_exit = true;
    m_elementAdded.notify_all();
    lock.unlock();

    //Wait until it has finished
    m_loop.join();
}

void Loop::loopFunc() {
    while(!m_exit){
        std::unique_lock<std::mutex> lock(m_mutex); //Adquire the mutex

        auto timeForNextEvent = m_scheduler.getTimeForNextEvent();

        if(timeForNextEvent == Duration::max()){
            //There are no pending events. Sleep
            m_elementAdded.wait(lock);
        } else {
            //Sleep until the moment arrives
            lock.unlock();
            std::this_thread::sleep_until(m_now + timeForNextEvent);
            lock.lock();

            m_now += timeForNextEvent;
            m_scheduler.addTime(timeForNextEvent);
        }
    }
}

}