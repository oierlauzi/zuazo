#include "Timing.h"

namespace Zuazo::Timing{

std::unique_ptr<TimingTable> timings;

int init(){
	timings=std::unique_ptr<TimingTable>(new TimingTable());
	return 0;
}

int end(){
	timings=std::unique_ptr<TimingTable>();
	return 0;
}

}
