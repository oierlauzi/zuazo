#include "Signal.h"

namespace Zuazo::Signal {

template <typename T>
inline void operator<<(ConsumerPad<T>& dst, SourcePad<T>& src){
    dst.setSource(&src);
}

template <typename T>
inline void operator<<(ConsumerPad<T>* dst, SourcePad<T>* src){
    if(dst && src) {
        dst->setSource(src);
    }
}

template <typename T>
inline void operator<<(ConsumerPad<T>& dst, NoSignal){
    dst.setSource(nullptr);
}

template <typename T>
inline void operator<<(ConsumerPad<T>* dst, NoSignal){
    if(dst) {
        dst->setSource(nullptr);
    }
}

}