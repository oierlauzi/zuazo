#pragma once

#include "SourcePad.h"
#include "ConsumerPad.h"

namespace Zuazo::Signal {

struct NoSignal {};
constexpr NoSignal noSignal;

template <typename T>
void operator<<(ConsumerPad<T>& dst, SourcePad<T>& src);

template <typename T>
void operator<<(ConsumerPad<T>* dst, SourcePad<T>* src);

template <typename T>
void operator<<(ConsumerPad<T>& dst, NoSignal);

template <typename T>
void operator<<(ConsumerPad<T>* dst, NoSignal);

}

#include "Signal.inl"