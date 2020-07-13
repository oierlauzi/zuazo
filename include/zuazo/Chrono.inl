#include "Chrono.h"

namespace Zuazo {

constexpr Duration getPeriod(const Rate& rate) {
	constexpr auto fromSec = Rate(1L) / Rate(Duration::period());
	return Duration(static_cast<Duration::rep>( (Rate(1L) / rate) * fromSec ));
}

constexpr Rate getRate(const Duration& period) {
	constexpr auto toSec = Rate(Duration::period());
	return Rate(1L) / ( Rate(period.count()) * toSec );
}

inline TimePoint now() {
	return std::chrono::time_point_cast<Duration>(Clock::now());
}

}