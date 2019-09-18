#pragma once

#include <memory>

namespace Zuazo::Stream {

template <typename T>
class SourcePad;

template <typename T>
class ConsumerPad {
public:
    ConsumerPad() = default;
    ConsumerPad(const ConsumerPad& other);
    ConsumerPad(ConsumerPad&& other);
    virtual ~ConsumerPad();

    void                                    setSource(SourcePad<T>* src);
    SourcePad<T>*                           getSource() const;
protected:
    static const std::shared_ptr<const T>   NO_SIGNAL;

    const std::shared_ptr<const T>&         get() const;
private:
    SourcePad<T>*                           m_source = nullptr;
};

}

#include "ConsumerPad.inl"