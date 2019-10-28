namespace Zuazo::Timing {

constexpr Period getPeriod(const Rate& rate) {
	Period::rep ticks = static_cast<Period::rep>( Rate(Period::period::den, Period::period::num) / rate );
	return Period(ticks);
}

constexpr Rate getRate(const Period& period) {
	return Rate(Duration::period::num, Duration::period::den) / Rate(period.count(), 1);
}

}