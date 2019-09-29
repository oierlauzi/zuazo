#include "MainThread.h"

namespace Zuazo::Timing {

inline void MainThread::lock(){
    m_mutex.lock();
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
inline std::shared_future<T> MainThread::execute(const std::function<T(Args...)>& func, Args&&... args) {
    //Create a future object
    std::shared_future<T> futur(
        std::async(std::launch::deferred, func, std::forward<Args>(args)...)
    );

    //Put it on the queue
    m_executions.emplace( [=] {
        futur.wait();
    });

    handleEvents();

    return futur;
}

inline void MainThread::waitForExecution() const {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_eventsHandled.wait(lock);
}

inline TimePoint getCurrentTime(){
    return mainThread->getCurrentTime();
}

inline Duration  getElapsed(){
    return mainThread->getElapsed();
}

}