#pragma once

#include "PadBase.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class SourcePad;

template <typename T>
class ConsumerPad : virtual public PadBase {
public:
    ConsumerPad() = default;
    ConsumerPad(std::string&& name);
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