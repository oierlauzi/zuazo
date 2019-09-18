#pragma once

#include "ConsumerPad.h"

#include <memory>

namespace Zuazo::Stream {


template <typename T>
class ConsumerPad;

template <typename T>
class Consumer : public ConsumerPad<T> {
public:
    struct BackupSignal;

    Consumer() = default; 
    Consumer(const Consumer& other) = default; 
    Consumer(Consumer&& other) = default; 
    virtual ~Consumer() = default; 

    void                                reset();
    const std::shared_ptr<const T>&     get() const;
    bool                                hasChanged() const;

    static BackupSignal                 onNoSignal;
private:   
    mutable std::shared_ptr<const T>    m_lastElement;

    const std::shared_ptr<const T>&     reqElement() const;
};

template <typename T>
class Consumer<T>::BackupSignal : public ConsumerPad<T> {
    friend Consumer<T>;
};

}

#include "Consumer.inl"