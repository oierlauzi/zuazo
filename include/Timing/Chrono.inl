namespace Zuazo::Timing {

constexpr Flicks getPeriod(const Rate& rate) {
	constexpr auto fromSec = 1 / Rate(Flicks::period());
	return Flicks(static_cast<Flicks::rep>( (1 / rate) * fromSec ));
}

constexpr Rate getRate(const Flicks& period) {
	constexpr auto toSec = Rate(Flicks::period());
	return 1 / ( period.count() * toSec );
}

inline TimePoint now() {
	return std::chrono::time_point_cast<Flicks>(Clock::now());
}

}