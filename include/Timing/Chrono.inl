#include "Chrono.h"

namespace Zuazo::Timing {

constexpr Duration getPeriod(const Rate& rate) {
    Duration::rep ticks = static_cast<Duration::rep>( Rate(Duration::period::den, Duration::period::num) / rate );
    return Duration(ticks);
}

constexpr Rate getRate(const Duration& period) {
    return Rate(Duration::period::num, Duration::period::den) / Rate(period.count(), 1);
}

}