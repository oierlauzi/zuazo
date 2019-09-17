#pragma once

#include <memory>
#include <set>

namespace Zuazo::Stream {

template <typename T>
class Consumer;

template <typename T>
class Source {
    friend Consumer<T>;
public:
    Source() = default;
    Source(const Source& other);
    Source(Source&& other);
    virtual ~Source();

    void                                    addConsumer(Consumer<T>& consumer);
    void                                    addConsumer(Consumer<T>* consumer);
    void                                    removeConsumer(Consumer<T>& consumer);
    void                                    removeConsumer(Consumer<T>* consumer);
    const std::set<Consumer<T>*>&           getConsumers() const;

    virtual const std::shared_ptr<const T>& get() const;
protected:
    void                                    reset();
    void                                    push(std::shared_ptr<const T>&& element);
private:
    std::set<Consumer<T>*>                  m_consumers;

    std::shared_ptr<const T>                m_lastElement;
};

template <typename T>
struct SourcePad : Source<T> {
    using Source<T>::Source;
    using Source<T>::reset;
    using Source<T>::push;
};

}

#include "Source.inl"