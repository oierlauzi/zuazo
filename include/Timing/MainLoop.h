#pragma once

#include "Scheduler.h"
#include "Chrono.h"
#include "../Zuazo.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

namespace Zuazo::Timing {

class MainLoop {
    friend void Zuazo::init();
    friend void Zuazo::end();
public:
    MainLoop(const MainLoop& other) = delete;
    virtual ~MainLoop();

    static MainLoop&                    getMainLoop();

    TimePoint                           getCurrentTime() const;
    Duration                            getElapsed() const;
    Scheduler&                          getScheduler();
    const Scheduler&                    getScheduler() const;
    std::mutex&                         getMutex();

    void                                handleEvents();
private:
    MainLoop();

    Scheduler                           m_scheduler;

    TimePoint                           m_now;
    Duration                            m_elapsed;

    std::atomic<bool>                   m_exit;
    std::thread                         m_thread;
    mutable std::mutex                  m_mutex;
    mutable std::condition_variable     m_handleEvents;

    void                                threadFunc();

    static std::unique_ptr<MainLoop>    s_mainLoop;
    static void                         init();
    static void                         end();
};

extern MainLoop& getMainLoop();
extern TimePoint getCurrentTime();
extern Duration  getElapsed();

}

#include "MainLoop.inl"