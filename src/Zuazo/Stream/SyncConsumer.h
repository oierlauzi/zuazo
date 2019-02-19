#pragma once

#include "../Timing/PeriodicUpdate.h"
#include "../UpdateOrder.h"
#include "Consumer.h"

namespace Zuazo::Stream{

template <typename T>
class SyncConsumer :
	public Consumer<T>,
	public Timing::PeriodicUpdate<UpdateOrder::OUTPUT>
{
public:
	using Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::PeriodicUpdate;

	virtual void open() override{
		Consumer<T>::open();
		Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::open();
	}

	virtual void close() override{
		Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::close();
		Consumer<T>::close();
	}
protected:
	using Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::setInterval;
	using Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::setRate;
	using Consumer<T>::get;
};

}
