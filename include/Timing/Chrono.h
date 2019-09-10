#pragma once

#include "../Math/Rational.h"

#include <chrono>

namespace Zuazo::Timing {

using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;
using Rate = Math::Rational<Duration::rep>;

constexpr Duration getPeriod(const Rate& rate);
constexpr Rate getRate(const Duration& period);

}

#include "Chrono.inl"