#pragma once

#include "Consumer.h"
#include "Source.h"

#include <memory>

namespace Zuazo::Stream{

/**
 * Dummy bidirectional pad
 */

template <typename T>
class DummyPad :
		public Source<T>,
		public Consumer<T>
{
public:
	std::shared_ptr<const T>	get() const override;

	void reset();
};

template <typename T>
inline std::shared_ptr<const T> DummyPad<T>::get() const{
	return Consumer<T>::get(); //That's all
}

template <typename T>
inline void DummyPad<T>::reset(){
	Source<T>::reset();
	Consumer<T>::reset();

}

}
