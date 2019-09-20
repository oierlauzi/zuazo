#pragma once

#include "PadBase.h"
#include "SourcePad.h"
#include "ConsumerPad.h"

namespace Zuazo::Signal {

template <typename T>
class DummyPad : 
    public SourcePad<T>, 
    public ConsumerPad<T> 
{
public:
    DummyPad() = default;
    DummyPad(std::string&& name);
    DummyPad(const DummyPad& other) = default;
    DummyPad(DummyPad&& other) = default;
    virtual ~DummyPad() = default;
private:
    virtual const std::shared_ptr<const T>& get() const override;
};

}

#include "DummyPad.inl"