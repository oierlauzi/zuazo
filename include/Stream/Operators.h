#pragma once

#include "Source.h"
#include "Consumer.h"

namespace Zuazo::Stream{

template <typename T>
inline Consumer<T>& operator<<(Consumer<T>& cons, const Source<T>& src){
	cons.setSource(&src);
    return cons;
}

template <typename T>
inline Consumer<T>& operator<<(Consumer<T>& cons, nullptr_t src){
	cons.setSource(nullptr);
    return cons;
}

}