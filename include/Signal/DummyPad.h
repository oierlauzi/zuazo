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
    DummyPad(PadBase::Direction dir = PadBase::Direction::NONE, std::string&& name = ""); 
    DummyPad(DummyPad&& other) = default;
    virtual ~DummyPad() = default;

    using PadBase::setName;
    using PadBase::setDirection;

    DummyPad& operator=(DummyPad&& other) = default;
private:
    virtual const std::shared_ptr<const T>& get() const final;
};


template <typename T>
class DummyInput : public DummyPad<T>{
    DummyInput(std::string&& name = "");
    DummyInput(DummyInput&& other) = default;
    virtual ~DummyInput() = default;

    DummyInput& operator=(DummyInput&& other) = default;
};

template <typename T>
class DummyOutput : public DummyPad<T>{
    DummyOutput(std::string&& name = "");
    DummyOutput(DummyOutput&& other) = default;
    virtual ~DummyOutput() = default;

    DummyOutput& operator=(DummyOutput&& other) = default;
};

}

#include "DummyPad.inl"