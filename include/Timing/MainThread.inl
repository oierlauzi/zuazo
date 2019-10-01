#include "MainThread.h"

namespace Zuazo::Timing {

inline void MainThread::lock(){
    m_mutex.lock();
}

inline bool MainThread::try_lock(){
    return m_mutex.try_lock();
}

inline void MainThread::unlock(){
    m_mutex.unlock();
}

inline const Scheduler& MainThread::getScheduler() const{
    return m_scheduler;
}

inline Scheduler& MainThread::getScheduler(){
    return m_scheduler;
}

inline TimePoint MainThread::getCurrentTime() const{
    return m_now;
}

inline Duration MainThread::getElapsed() const{
    return m_elapsed;
}

inline void MainThread::handleEvents(){
    m_handleEvents.notify_all();
}

template<typename T, typename... Args>
inline T MainThread::execute(const std::function<T(Args...)>& func, Args&&... args) {
    std::unique_lock<std::mutex> lock(m_mutex);
    //Create a future object
    std::future<T> futur = std::async(std::launch::deferred, func, std::forward<Args>(args)...);

    //Put it on the queue
    m_execution = [&futur] {
        futur.wait();
    };

    //Wait until it gets completed
    handleEvents();
    m_eventsHandled.wait(lock);

    //Return result
    return futur.get();
}

inline TimePoint getCurrentTime(){
    return mainThread->getCurrentTime();
}

inline Duration  getElapsed(){
    return mainThread->getElapsed();
}

}