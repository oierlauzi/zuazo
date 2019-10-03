#include "MainThread.h"

namespace Zuazo::Timing {

inline void MainThread::init(){
    s_singleton = std::make_unique<MainThread>();
}

inline void MainThread::end(){
    s_singleton.reset();
}

inline MainThread& MainThread::getMainThread(){
    return *s_singleton;
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
inline T MainThread::execute(const std::function<T(Args...)>& func, Args... args) {
    std::unique_lock<std::mutex> lock(m_mutex);
    //Create a future object
    std::future<T> futur = std::async(std::launch::deferred, func, args...);

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



inline MainThread& getMainThread(){
    return MainThread::getMainThread();
}

inline TimePoint getCurrentTime(){
    return getMainThread().getCurrentTime();
}

inline Duration getElapsed(){
    return getMainThread().getElapsed();
}

}