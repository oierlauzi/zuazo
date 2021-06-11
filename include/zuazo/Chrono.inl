#include "Chrono.h"

#include "StringConversions.h"

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



constexpr std::string_view toString(DurationUnit unit) noexcept {
	switch(unit){

	case DurationUnit::second: 	return "s";

	default: 					return "";
	}
}

inline bool fromString(std::string_view str, DurationUnit& unit) {
	return enumFromString(str, unit);
}

inline std::ostream& operator<<(std::ostream& os, DurationUnit unit) {
	return os << toString(unit);
}



inline std::ostream& operator<<(std::ostream& os, Duration dur) {
	const auto seconds = std::chrono::duration_cast<Seconds>(dur);
	return os << seconds.count() << DurationUnit::second;
}

inline size_t fromString(std::string_view str, Duration& dur) noexcept {
	size_t result = 0;
	size_t read;

	//Get the count
	double count;
	result += (read = fromString(str.substr(result), count));
	if(!read) {
		return 0;
	}

	//Get the units
	DurationUnit unit;
	result += (read = fromString(str.substr(result), unit));
	if(!read) {
		return 0;
	}

	//Elaborate the result
	dur = std::chrono::duration_cast<Duration>(Seconds(count));
	return result;
}





namespace Utils {

constexpr DurationUnit EnumTraits<DurationUnit>::first() noexcept {
	return DurationUnit::none + static_cast<DurationUnit>(1);
}

constexpr DurationUnit EnumTraits<DurationUnit>::last() noexcept {
	return DurationUnit::count - static_cast<DurationUnit>(1);
}

}

}