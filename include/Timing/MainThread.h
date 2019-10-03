#pragma once

#include "Scheduler.h"
#include "Chrono.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

namespace Zuazo::Timing {

class MainThread {
    friend std::unique_ptr<MainThread> std::make_unique<MainThread>();
    friend ScheduledEvent;
public:
    MainThread(const MainThread& other) = delete;
    virtual ~MainThread();

    static void                         init();
    static void                         end();
    static MainThread&                  getMainThread();

    TimePoint                           getCurrentTime() const;
    Duration                            getElapsed() const;
    
    void                                handleEvents();

    template<typename T, typename... Args>
    T                                   execute(const std::function<T(Args...)>& func, Args... args);
private:
    MainThread();

    Scheduler                           m_scheduler;
    std::function<void()>               m_execution;

    TimePoint                           m_now;
    Duration                            m_elapsed;

    volatile bool                       m_exit;
    std::thread                         m_thread;
    mutable std::mutex                  m_mutex;

    mutable std::condition_variable     m_handleEvents;
    mutable std::condition_variable     m_eventsHandled;

    void                                threadFunc();
    void                                setup();
    void                                cleanup();

    static std::unique_ptr<MainThread>  s_singleton;
};

extern MainThread& getMainThread();
extern TimePoint getCurrentTime();
extern Duration  getElapsed();

}

#include "MainThread.inl"