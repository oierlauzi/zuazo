#pragma once

#include "InputPad.h"

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
    using PadBase::setDirection;

    struct BackupSignal;

    void                                setHold(bool hold);
    bool                                getHold() const;

    void                                reset();
    const std::shared_ptr<const T>&     get() const;
    bool                                hasChanged() const;

    static BackupSignal                 backupSignal;

protected:
    static constexpr Timing::EventBase::Priority PRIORITY = 
                    std::numeric_limits<Timing::EventBase::Priority>::max();

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