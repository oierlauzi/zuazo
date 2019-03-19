#pragma once

#include <memory>

#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

/**
 * Dummy bidirectional pad
 */

template <typename T, typename Q>
class DummyBidirPad :
		public Source<T>,
		public Consumer<T>
{
	friend Q;
public:
	using Consumer<T>::reset;
	std::shared_ptr<const T>	get() const override;
};

template <typename T, typename Q>
inline std::shared_ptr<const T> DummyBidirPad<T, Q>::get() const{
	return Consumer<T>::get(); //That's all
}

/**
 * Dummy input pad
 */

template <typename T, typename Q>
class DummyInputPad :
		private Source<T>,
		public Consumer<T>
{
	friend Q;
public:
	using Consumer<T>::reset;
	std::shared_ptr<const T>	get() const override;
};

template <typename T, typename Q>
inline std::shared_ptr<const T> DummyInputPad<T, Q>::get() const{
	return Consumer<T>::get(); //That's all
}


/**
 * Dummy output pad
 */

template <typename T, typename Q>
class DummyOutputPad :
		public Source<T>,
		private Consumer<T>
{
	friend Q;
public:
	using Consumer<T>::reset;
	std::shared_ptr<const T>	get() const override;
};

template <typename T, typename Q>
inline std::shared_ptr<const T> DummyOutputPad<T, Q>::get() const{
	return Consumer<T>::get(); //That's all
}

}
