#pragma once

#include "InputPad.h"
#include "../Timing/ScheduledEvent.h"

#include <memory>
#include <limits>

namespace Zuazo::Signal {


template <typename T>
class InputPad;

template <typename T>
class Input : public InputPad<T> {
public:
    using InputPad<T>::InputPad;
    using PadBase::setName;

    Input&                              operator=(const Input& other) = default;
    Input&                              operator=(Input&& other) = default;

    struct BackupSignal;

    void                                setHold(bool hold);
    bool                                getHold() const;

    void                                reset();
    const std::shared_ptr<const T>&     get() const;
    bool                                hasChanged() const;

    static BackupSignal                 backupSignal;

protected:
    static constexpr Timing::ScheduledEvent::Priority PRIORITY = 3;

private:   
    mutable std::shared_ptr<const T>    m_lastElement;
    bool                                m_hold = false;
};

template <typename T>
class Input<T>::BackupSignal : public InputPad<T> {
    friend Input<T>;
};

}

#include "Input.inl"