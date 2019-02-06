#include "TimeUnit.h"

using namespace Zuazo::Timing;

const int64_t TimeUnit::TIME_UNITS_PER_SECOND=Timing::TimeUnit( std::chrono::seconds(1) ).count();
