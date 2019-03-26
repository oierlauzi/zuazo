#pragma once

#include "Source.h"

#include <cstddef>
#include <memory>
#include <mutex>

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer{
	friend Source<T>;
public:
	Consumer();
	Consumer(const Consumer& other);
	virtual ~Consumer();

	void								setSource(const Source<T>* src);
	Consumer&							operator<<(const Source<T>& src);
	Consumer&							operator<<(std::nullptr_t ptr);

protected:
	std::shared_ptr<const T>			get() const;
	bool								hasChanged() const;
	void								reset();

private:
	const Source<T>*					m_source;

	mutable std::mutex					m_mutex;

	mutable std::shared_ptr<const T>	m_lastElement;
	std::shared_ptr<const T>			reqElement() const;
};



template <typename T, typename Q>
class ConsumerPad :
		public Consumer<T>
{
	friend Q;
};



template <typename T>
inline Consumer<T>::Consumer(){
	m_source=nullptr;
}

template <typename T>
inline Consumer<T>::Consumer(const Consumer& other) : Consumer(){
	setSource(other.m_source);
}

template <typename T>
inline Consumer<T>::~Consumer(){
	setSource(nullptr);
}

template <typename T>
inline void Consumer<T>::setSource(const Source<T>* src){
	std::lock_guard<std::mutex> lock(m_mutex);

	//Detach consumer from the previous source
	if(m_source)
		m_source->m_consumers.erase(this);

	//Attach the new consumer
	m_source=src;
	if(m_source)
		m_source->m_consumers.insert(this);
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(const Source<T>& src){
	setSource(&src);
	return *this;
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(std::nullptr_t ptr){
	setSource(nullptr);
	return *this;
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
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_source)
		return m_source->get();
	else
		return std::shared_ptr<const T>(); //No source, return an empty ptr
}

}