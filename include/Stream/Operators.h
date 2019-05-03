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

template <typename T>
inline const Source<T>& operator>>(const Source<T>& src, Consumer<T>& cons){
	cons.setSource(&src);
    return src;
}

template <typename T>
inline Source<T>& operator>>(Source<T>& src, Consumer<T>& cons){
	cons.setSource(&src);
    return src;
}

template <typename T>
inline nullptr_t operator>>(nullptr_t src, Consumer<T>& cons){
	cons.setSource(nullptr);
    return nullptr;
}

}