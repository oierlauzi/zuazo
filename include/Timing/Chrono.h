#pragma once

#include "../Math/Rational.h"

#include <chrono>

namespace Zuazo::Timing {

using Clock = std::chrono::steady_clock;

//From https://github.com/facebookarchive/Flicks
using Flicks = std::chrono::duration<std::chrono::nanoseconds::rep, std::ratio<1, 705600000>>;
using TimePoint = std::chrono::time_point<Clock, Flicks>;
using Rate = Math::Rational<Flicks::rep>;

template<intmax_t rate_num, intmax_t rate_den = 1>
using Frames = std::chrono::duration<	Flicks::rep, 
										std::ratio<rate_den, rate_num>>;

constexpr Flicks getPeriod(const Rate& rate);
constexpr Rate getRate(const Flicks& period);

TimePoint now();

}

#include "Chrono.inl"