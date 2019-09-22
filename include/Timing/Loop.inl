#include "Loop.h"

namespace Zuazo::Timing {

template <typename T>
inline void Loop::addEvent(const T& evnt) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_scheduler.addEvent(evnt);
    m_elementAdded.notify_one();
}

template <typename T>
inline void Loop::removeEvent(const T& evnt) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_scheduler.removeEvent(evnt);
}

inline TimePoint Loop::getNow(){
    return m_now;
}

}