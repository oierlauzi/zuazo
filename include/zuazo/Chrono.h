#pragma once

#include "Macros.h"
#include "Math/Rational.h"

#include <chrono>

namespace Zuazo {

enum class DurationUnit : int {
	none = -1,

	second,

	count,
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(DurationUnit)
ZUAZO_ENUM_COMP_OPERATORS(DurationUnit)



//From https://github.com/facebookarchive/Flicks
using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<std::chrono::nanoseconds::rep, std::ratio<1, 705600000>>;
using TimePoint = std::chrono::time_point<Clock, Duration>;
using Rate = Math::Rational<Duration::rep>;
using Seconds = std::chrono::duration<double, std::ratio<1, 1>>;

template<intmax_t rate_num, intmax_t rate_den = 1>
using Frames = std::chrono::duration<	Duration::rep, 
										std::ratio<rate_den, rate_num>>; //Note that they are inverted

constexpr Duration getPeriod(const Rate& rate) noexcept;
constexpr Rate getRate(const Duration& period) noexcept;

TimePoint now() noexcept;



constexpr std::string_view toString(DurationUnit unit) noexcept;
bool fromString(std::string_view str, DurationUnit& unit);
std::ostream& operator<<(std::ostream& os, DurationUnit unit);


std::ostream& operator<<(std::ostream& os, Duration dur);
size_t fromString(std::string_view str, Duration& dur) noexcept;





namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<DurationUnit> {
	static constexpr DurationUnit first() noexcept;
	static constexpr DurationUnit last() noexcept;
};

}


}

#include "Chrono.inl"