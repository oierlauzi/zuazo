#pragma once

#include "OutputPad.h"
#include "InputPad.h"

namespace Zuazo::Signal {

struct NoSignal {};
constexpr NoSignal noSignal;

template <typename T>
void operator<<(InputPad<T>& dst, OutputPad<T>& src);

template <typename T>
void operator<<(InputPad<T>& dst, NoSignal);

}

#include "Signal.inl"