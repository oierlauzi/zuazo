#pragma once

#include <chrono>

#include "../Math/Rational.h"

namespace Zuazo::Time {

typedef std::chrono::steady_clock Clock;
typedef Clock::duration Duration;
typedef Clock::time_point TimePoint;
typedef Clock::rep Rep;

constexpr Math::Rational CLOCK_RESOLUTION(Clock::period::num, Clock::period::den);

constexpr Duration rate_2_period(const Math::Rational& rate);
constexpr Math::Rational period_2_rate(Duration dur);

}

#include "Units.inl"
