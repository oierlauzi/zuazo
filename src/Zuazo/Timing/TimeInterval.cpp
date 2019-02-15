#include "TimeInterval.h"

using namespace Zuazo::Timing;

const int64_t TimeInterval::TIME_UNITS_PER_SECOND=Timing::TimeInterval( std::chrono::seconds(1) ).count();
