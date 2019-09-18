#pragma once

#include "SourcePad.h"

#include <memory>

namespace Zuazo::Stream {

template <typename T>
class Source : public SourcePad<T> {
public:
    Source() = default;
    Source(const Source& other) = default;
    Source(Source&& other) = default;
    virtual ~Source() = default;

    void                                    reset();
    void                                    push(std::shared_ptr<const T>&& element);
private:
    std::shared_ptr<const T>                m_lastElement;

    virtual const std::shared_ptr<const T>& get() const override;
};

}

#include "Source.inl"