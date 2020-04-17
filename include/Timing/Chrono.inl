namespace Zuazo::Timing {

constexpr Duration getPeriod(const Rate& rate) {
	constexpr auto fromSec = 1 / Rate(Duration::period());
	return Duration(static_cast<Duration::rep>( (1 / rate) * fromSec ));
}

constexpr Rate getRate(const Duration& period) {
	constexpr auto toSec = Rate(Duration::period());
	return 1 / ( period.count() * toSec );
}

inline TimePoint now() {
	return std::chrono::time_point_cast<Duration>(Clock::now());
}

}