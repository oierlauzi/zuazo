#pragma once

#include <chrono>

#include "../Utils/Rational.h"

#define TIME_UNIT microseconds

namespace Zuazo::Timing{

class TimeInterval : public std::chrono::TIME_UNIT{
public:
	using std::chrono::TIME_UNIT::duration; //Use default constructor/destructors

	TimeInterval(const Utils::Rational& rat) : std::chrono::TIME_UNIT(
			rat.den ? rat.num * TIME_UNITS_PER_SECOND / rat.den : 0
	){}

	TimeInterval(const TimeInterval& other) : std::chrono::TIME_UNIT(
			static_cast<std::chrono::TIME_UNIT>(other)
	){}

	TimeInterval(const std::chrono::TIME_UNIT& other) : std::chrono::TIME_UNIT(other){

	}

	~TimeInterval()=default;

	operator Utils::Rational() const{
		return Utils::Rational(count(), TIME_UNITS_PER_SECOND);
	}
	double seconds(){
		return (double)count()/TIME_UNITS_PER_SECOND;
	}

	static constexpr int64_t TIME_UNITS_PER_SECOND=std::chrono::TIME_UNIT( std::chrono::seconds(1) ).count();
};
}
