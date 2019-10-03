#pragma once

#include "OutputPad.h"
#include "../Timing/ScheduledEvent.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Output : public OutputPad<T> {
public:
    using OutputPad<T>::OutputPad;
    using PadBase::setName;
    using PadBase::setDirection;

    Output&                                 operator=(Output&& other) = default;

    void                                    reset();
    void                                    push(std::shared_ptr<const T>&& element);

protected:
    static constexpr Timing::ScheduledEvent::Priority PRIORITY = 0;

private:
    std::shared_ptr<const T>                m_lastElement;

    virtual const std::shared_ptr<const T>& get() const override;
};

}

#include "Output.inl"