#pragma once

#include "OutputPad.h"
#include "../Timing/EventBase.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Output : public OutputPad<T> {
public:
    using OutputPad<T>::OutputPad;
    using PadBase::setName;
    using PadBase::setDirection;

    void                                    reset();
    void                                    push(std::shared_ptr<const T>&& element);

protected:
    static constexpr Timing::EventBase::Priority PRIORITY = 1;

private:
    std::shared_ptr<const T>                m_lastElement;

    virtual const std::shared_ptr<const T>& get() const override;
};

}

#include "Output.inl"