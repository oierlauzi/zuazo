#pragma once

#include "Source.h"

#include <memory>

namespace Zuazo::Stream {

template <typename T>
class Consumer {
public:
    Consumer(); 
    Consumer(const Consumer& other);
    Consumer(Consumer&& other);
    virtual ~Consumer();

    void                                setSource(Source<T>& src);
    void                                setSource(Source<T>* src);
protected:
    const std::shared_ptr<const T>&     get() const;
    bool                                hasChanged() const;
private:
    Source<T>*                          m_source;
    
    mutable std::shared_ptr<const T>    m_lastElement;
};

template <typename T>
struct ConsumerPad : Consumer<T>{
    using Consumer<T>::Consumer;
    using Consumer<T>::get;
    using Consumer<T>::hasChanged;
};

}

#include "Consumer.inl"