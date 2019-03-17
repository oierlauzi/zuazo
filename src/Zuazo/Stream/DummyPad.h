#pragma once

#include "Consumer.h"
#include "LazySource.h"

namespace Zuazo::Stream{

template <typename T>
class DummyPad :
		public Source<T>,
		public Consumer<T>
{
public:
	using Consumer<T>::reset;
	std::shared_ptr<const T>	get() const override;
};

template <typename T>
std::shared_ptr<const T> DummyPad<T>::get() const{
	return Consumer<T>::get(); //That's all
}

}
