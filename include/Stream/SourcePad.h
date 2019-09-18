#pragma once

#include <memory>
#include <set>

namespace Zuazo::Stream {

template <typename T>
class ConsumerPad;

template <typename T>
class SourcePad {
    friend ConsumerPad<T>;
public:
    SourcePad() = default;
    SourcePad(const SourcePad& other);
    SourcePad(SourcePad&& other);
    virtual ~SourcePad();

    const std::set<ConsumerPad<T>*>&        getConsumers() const;
private:
    std::set<ConsumerPad<T>*>               m_consumers;

    virtual const std::shared_ptr<const T>& get() const = 0;
};

}

#include "SourcePad.inl"