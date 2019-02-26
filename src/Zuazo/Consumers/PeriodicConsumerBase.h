#pragma once

#include "../Timing/PeriodicUpdate.h"
#include "../UpdateOrder.h"
#include "ConsumerBase.h"

namespace Zuazo::Consumers{

class PeriodicConsumerBase :
	public ConsumerBase,
	public Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >
{
public:
	using Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >::PeriodicUpdate;

	virtual void open() override;
	virtual void close() override;
protected:
	using Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >::setInterval;
	using Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::OUTPUT >::setRate;
	using ConsumerBase::get;
};

}
