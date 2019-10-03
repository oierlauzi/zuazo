#pragma once

#include "Output.h"
#include "../Timing/LazyEvent.h"

namespace Zuazo::Signal {

template <typename T>
class LazyOutput : public Output<T> {
public:
    LazyOutput(Timing::LazyEvent& evnt, std::string&& name = "");
    LazyOutput(LazyOutput&& other) = default;
    virtual ~LazyOutput() = default;

    LazyOutput& operator=(LazyOutput&& other) = default;

    virtual std::shared_ptr<const T>& get() const override;
private:
    Timing::LazyEvent& m_owner;
};

}

#include "LazyOutput.inl"