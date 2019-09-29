#pragma once

#include "Scheduler.h"
#include "Chrono.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace Zuazo::Timing {

class MainThread {
public:
    MainThread();
    MainThread(const MainThread& other) = delete;
    virtual ~MainThread();

    void                                lock();
    void                                unlock();

    const Scheduler&                    getScheduler() const;
    Scheduler&                          getScheduler();

    TimePoint                           getCurrentTime() const;
    Duration                            getElapsed() const;
    
    void                                handleEvents();

    template<typename T, typename... Args>
    std::shared_future<T>               execute(const std::function<T(Args...)>& func, Args&&... args);
    void                                waitForExecution() const;
private:
    Scheduler                           m_scheduler;
    std::queue<std::function<void()>>   m_executions;

    TimePoint                           m_now;
    Duration                            m_elapsed;

    volatile bool                       m_exit;
    std::thread                         m_thread;
    mutable std::mutex                  m_mutex;

    mutable std::condition_variable     m_handleEvents;
    mutable std::condition_variable     m_eventsHandled;

    void                                threadFunc();
};

extern std::unique_ptr<MainThread> mainThread;

TimePoint getCurrentTime();
Duration  getElapsed();

}

#include "MainThread.inl"