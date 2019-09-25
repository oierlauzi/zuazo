#include "Signal.h"

namespace Zuazo::Signal {

template <typename T>
inline void operator<<(InputPad<T>& dst, OutputPad<T>& src){
    dst.setSource(&src);
}

template <typename T>
inline void operator<<(InputPad<T>& dst, NoSignal){
    dst.setSource(nullptr);
}

}