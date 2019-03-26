#pragma once

#include <chrono>
#include <type_traits>

#include "TimeInterval.h"

#define TIME_CLOCK steady_clock

namespace Zuazo::Utils{

class TimePoint : public std::chrono::TIME_CLOCK::time_point{
public:
	using std::chrono::TIME_CLOCK::time_point::time_point; //Use default constructor/destructors

	TimePoint(const std::chrono::TIME_CLOCK::time_point& other) :
		std::chrono::TIME_CLOCK::time_point(other)
	{
	}

	TimePoint(const TimePoint& other) :
		std::chrono::TIME_CLOCK::time_point(other)
	{
	}

	~TimePoint()=default;

	TimeInterval operator -(const TimePoint& other) const{
		return std::chrono::duration_cast<std::chrono::TIME_UNIT>(
				static_cast<std::chrono::TIME_CLOCK::time_point> (*this) - static_cast<std::chrono::TIME_CLOCK::time_point> (other)
				);
	}

	operator bool(){
		return time_since_epoch().count();
	}

	static TimePoint now(){
		return std::chrono::TIME_CLOCK::now();
	}
};

}
