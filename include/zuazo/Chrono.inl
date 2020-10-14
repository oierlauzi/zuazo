#include "Chrono.h"

namespace Zuazo {

constexpr Duration getPeriod(const Rate& rate) noexcept {
	constexpr auto fromSec = Rate(1L) / Rate(Duration::period());
	return Duration(static_cast<Duration::rep>( (Rate(1L) / rate) * fromSec ));
}

constexpr Rate getRate(const Duration& period) noexcept {
	constexpr auto toSec = Rate(Duration::period());
	return Rate(1L) / ( Rate(period.count()) * toSec );
}

inline TimePoint now() noexcept {
	return std::chrono::time_point_cast<Duration>(Clock::now());
}

namespace Utils {

constexpr Duration lowest(const Any<Duration>&) noexcept {
	return Duration::min();
}

constexpr Duration highest(const Any<Duration>&) noexcept {
	return Duration::max();
}

}

}