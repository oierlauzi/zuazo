#pragma once

#include "Math/Rational.h"

#include <chrono>

namespace Zuazo {

using Clock = std::chrono::steady_clock;

//From https://github.com/facebookarchive/Flicks
using Duration = std::chrono::duration<std::chrono::nanoseconds::rep, std::ratio<1, 705600000>>;
using TimePoint = std::chrono::time_point<Clock, Duration>;
using Rate = Math::Rational<Duration::rep>;

template<intmax_t rate_num, intmax_t rate_den = 1>
using Frames = std::chrono::duration<	Duration::rep, 
										std::ratio<rate_den, rate_num>>; //Note that they are inverted

constexpr Duration getPeriod(const Rate& rate);
constexpr Rate getRate(const Duration& period);

TimePoint now();

}

#include "Chrono.inl"