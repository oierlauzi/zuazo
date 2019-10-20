#include "MainLoop.h"

namespace Zuazo::Timing {

inline void MainLoop::init(){
    s_mainLoop = std::make_unique<MainLoop>();
}

inline void MainLoop::end(){
    s_mainLoop.reset();
}

inline MainLoop& MainLoop::getMainLoop(){
    return *s_mainLoop;
}


inline TimePoint MainLoop::getCurrentTime() const{
    return m_now;
}

inline Duration MainLoop::getElapsed() const{
    return m_elapsed;
}

inline Scheduler& MainLoop::getScheduler(){
    return m_scheduler;
}

inline const Scheduler& MainLoop::getScheduler() const{
    return m_scheduler;
}

inline void MainLoop::lock(){
    m_mutex.lock();
}

inline void MainLoop::unlock(){
    m_mutex.unlock();
}

inline void MainLoop::handleEvents(){
    m_handleEvents.notify_all();
}





inline MainLoop& getMainLoop(){
    return MainLoop::getMainLoop();
}

inline TimePoint getCurrentTime(){
    return getMainLoop().getCurrentTime();
}

inline Duration getElapsed(){
    return getMainLoop().getElapsed();
}

}