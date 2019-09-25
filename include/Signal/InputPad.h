#pragma once

#include "PadBase.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class OutputPad;

template <typename T>
class InputPad : virtual public PadBase {
public:
    InputPad(std::string&& name);
    InputPad(const InputPad& other) = delete;
    virtual ~InputPad();

    void                                    setSource(OutputPad<T>* src);
    OutputPad<T>*                           getSource() const;
protected:
    static const std::shared_ptr<const T>   NO_SIGNAL;

    const std::shared_ptr<const T>&         get() const;
private:
    OutputPad<T>*                           m_source = nullptr;
};

}

#include "InputPad.inl"