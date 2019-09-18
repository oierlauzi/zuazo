#include "Signal.h"

namespace Zuazo::Stream {

template <typename T>
inline void operator<<(ConsumerPad<T>& dst, SourcePad<T>& src){
    dst.setSource(&src);
}

template <typename T>
inline void operator<<(ConsumerPad<T>& dst, NoSignal){
    dst.setSource(nullptr);
}

}