#pragma once

#include "../Updateables/PeriodicUpdate.h"
#include "../Updateables/UpdateOrder.h"
#include "Consumer.h"

namespace Zuazo::Stream{

template <typename T>
class PeriodicConsumer :
		public Consumer<T>,
		public Updateables::PeriodicUpdate<Updateables::UPDATE_ORDER_CONSUMER>
{
public:
	using Updateables::PeriodicUpdate<Updateables::UPDATE_ORDER_CONSUMER>::PeriodicUpdate;

	void open() override{
		Updateables::PeriodicUpdate<Updateables::UPDATE_ORDER_CONSUMER>::open();
		Consumer<T>::open();
	}

	void close() override{
		Consumer<T>::open();
		Updateables::PeriodicUpdate<Updateables::UPDATE_ORDER_CONSUMER>::close();
	}
};

}
