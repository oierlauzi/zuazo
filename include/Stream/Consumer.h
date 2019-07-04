#pragma once

#include "Source.h"

#include <cstddef>
#include <memory>

namespace Zuazo::Stream{

template <typename T>
class Consumer{
public:
	Consumer();
	Consumer(const Consumer& other) = default;
	Consumer(Consumer&& other) = default;
	virtual ~Consumer() = default;

	void								setSource(const Source<T>* src);
	friend Consumer&					operator<<(Consumer<T>& cons, const Source<T>& src);
protected:
	std::shared_ptr<const T>			get() const;
	Source<T>::id_t						getId() const;
	bool								hasChanged() const;
	void								reset();

private:
	const Source<T>*					m_source;

	mutable Source<T>::id_t				m_lastId;
};

template <typename T>
class ConsumerPad :
		public Consumer<T>
{
public:
	using Consumer<T>::get;
	using Consumer<T>::hasChanged;
	using Consumer<T>::reset;
	using Consumer<T>::Consumer;
};

}
