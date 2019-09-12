#pragma once

#include "../Math/Rational.h"

#include <chrono>

namespace Zuazo::Timing {

using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;
using TimePoint = Clock::time_point;

using Period = Duration;
using Rate = Math::Rational<Period::rep>;

constexpr Period getPeriod(const Rate& rate);
constexpr Rate getRate(const Period& period);

}

#include "Chrono.inl"