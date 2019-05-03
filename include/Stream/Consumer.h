#pragma once

#include "Source.h"

#include <cstddef>
#include <memory>
#include <mutex>

namespace Zuazo::Stream{

template <typename T>
class Consumer{
public:
	Consumer();
	Consumer(const Consumer& other)=default;
	Consumer(Consumer&& other)=default;
	virtual ~Consumer()=default;

	void								setSource(const Source<T>* src);
protected:
	std::shared_ptr<const T>			get() const;
	bool								hasChanged() const;
	void								reset();

private:
	const Source<T>*					m_source;

	mutable std::shared_ptr<const T>	m_lastElement;
	std::shared_ptr<const T>			reqElement() const;
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



template <typename T>
inline Consumer<T>::Consumer() :
	m_source(nullptr)
{
}

template <typename T>
inline void Consumer<T>::setSource(const Source<T>* src){
	m_source=src;
}

template <typename T>
inline std::shared_ptr<const T> Consumer<T>::get() const{
	std::shared_ptr<const T> el=reqElement();
	m_lastElement=el;
	return el;
}

template <typename T>
inline bool Consumer<T>::hasChanged() const{
	return m_lastElement != reqElement();

}

template <typename T>
inline void Consumer<T>::reset(){
	m_lastElement=std::shared_ptr<const T>();
}

template <typename T>
inline std::shared_ptr<const T> Consumer<T>::reqElement() const{
	return m_source ? m_source->get() : std::shared_ptr<const T>();
}

}
