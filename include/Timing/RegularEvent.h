#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class RegularEvent : public EventBase {
public:
    RegularEvent(Priority prior);
    RegularEvent(const RegularEvent& other) = delete;
    ~RegularEvent() = default;

    virtual void        enable() override;
    virtual void        disable() override;
protected:

private:

};

}