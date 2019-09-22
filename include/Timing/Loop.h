#pragma once

#include "Scheduler.h"
#include "Chrono.h"

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace Zuazo::Timing {

class Loop {
public:
    Loop();
    Loop(const Loop& other) = delete;
    Loop(Loop&& other) = delete;
    ~Loop();

    template <typename T>
    void                    addEvent(const T& evnt);
    template <typename T>
    void                    removeEvent(const T& evnt);

    TimePoint               getNow();
private:
    Scheduler               m_scheduler;
    TimePoint               m_now;

    volatile bool           m_exit;
    std::thread             m_loop;
    std::mutex              m_mutex;
    std::condition_variable m_elementAdded;

    void                    loopFunc();
};

extern std::unique_ptr<Loop> mainLoop;

}

#include "Loop.inl"