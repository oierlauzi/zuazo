#include "PeriodicConsumerBase.h"

using namespace Zuazo::Stream;

void PeriodicConsumerBase::open(){
	ConsumerBase::open();
	Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >::open();
}

void PeriodicConsumerBase::close(){
	Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >::close();
	ConsumerBase::close();
}
