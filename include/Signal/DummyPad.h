#pragma once

#include "PadBase.h"
#include "OutputPad.h"
#include "InputPad.h"

namespace Zuazo::Signal {

template <typename T>
class DummyPad : 
    public OutputPad<T>, 
    public InputPad<T>
{
public:
    DummyPad(std::string&& name, PadBase::Direction dir); 
    DummyPad(const DummyPad& other) = delete;
    virtual ~DummyPad() = default;

    using PadBase::setName;
    using PadBase::setDirection;
private:
    virtual const std::shared_ptr<const T>& get() const override;
};


template <typename T>
class DummyInput : public DummyPad<T>{
    DummyInput(std::string&& name);
    DummyInput(const DummyInput& other) = delete;
    virtual ~DummyInput() = default;
};

template <typename T>
class DummyOutput : public DummyPad<T>{
    DummyOutput(std::string&& name);
    DummyOutput(const DummyOutput& other) = delete;
    virtual ~DummyOutput() = default;
};

}

#include "DummyPad.inl"