#pragma once

#include "PadBase.h"

#include <memory>
#include <set>

namespace Zuazo::Signal {

template <typename T>
class InputPad;

template <typename T>
class OutputPad : virtual public PadBase {
    friend InputPad<T>;
public:
    OutputPad(std::string&& name);
    OutputPad(const OutputPad& other) = delete;
    virtual ~OutputPad();

    const std::set<InputPad<T>*>&        getConsumers() const;
private:
    std::set<InputPad<T>*>               m_consumers;

    virtual const std::shared_ptr<const T>& get() const = 0;
};

}

#include "OutputPad.inl"