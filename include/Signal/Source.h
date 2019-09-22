#pragma once

#include "SourcePad.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Source : public SourcePad<T> {
public:
    using SourcePad<T>::SourcePad;
    using PadBase::setName;

    void                                    reset();
    void                                    push(std::shared_ptr<const T>&& element);
private:
    std::shared_ptr<const T>                m_lastElement;

    virtual const std::shared_ptr<const T>& get() const override;
};

}

#include "Source.inl"