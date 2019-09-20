#pragma once

#include "ConsumerPad.h"

#include <memory>

namespace Zuazo::Signal {


template <typename T>
class ConsumerPad;

template <typename T>
class Consumer : public ConsumerPad<T> {
public:
    struct BackupSignal;
    using ConsumerPad<T>::ConsumerPad;

    void                                setHold(bool hold);
    bool                                getHold() const;

    void                                reset();
    const std::shared_ptr<const T>&     get() const;
    bool                                hasChanged() const;

    static BackupSignal                 backupSignal;
private:   
    mutable std::shared_ptr<const T>    m_lastElement;
    bool                                m_hold = false;
};

template <typename T>
class Consumer<T>::BackupSignal : public ConsumerPad<T> {
    friend Consumer<T>;
};

}

#include "Consumer.inl"