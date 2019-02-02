#pragma once

#include <memory>

#include "TimingTable.h"

namespace Zuazo::Timing{

extern int init();
extern int end();

extern std::unique_ptr<TimingTable> timings;

}
