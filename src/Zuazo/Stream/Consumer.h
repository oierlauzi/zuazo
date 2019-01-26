#pragma once

#include <stddef.h>
#include <memory>
#include <mutex>

#include "../Utils/Rational.h"
#include "../Timing.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer : public Updateable{
	friend Source<T>;
public:
	Consumer();
	Consumer(const Utils::Rational& rat);
	Consumer(const Consumer<T>& other);
	virtual ~Consumer();

	void						setSource(Source<T>* src);
	Consumer<T>&				operator<<(Source<T>&);
	Consumer<T>&				operator<<(std::nullptr_t ptr);
	virtual bool 				isActive() const { return true; }
protected:
	std::shared_ptr<const T>	get();
private:
	Source<T>*					m_source;
};

/************************
 * FUNCTION DEFINITIONS	*
 ************************/
/*
 * CONSTRUCTOR AND DESTRUCTOR
 */

template <typename T>
inline Consumer<T>::Consumer() : Updateable(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_source=nullptr;
}

template <typename T>
inline Consumer<T>::Consumer(const Utils::Rational& rat) : Updateable(rat){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_source=nullptr;
}

template <typename T>
inline Consumer<T>::Consumer(const Consumer<T>& other) : Consumer<T>::Consumer(other.getRate()){
	setSource(other.m_source);
}

template <typename T>
inline Consumer<T>::~Consumer(){
	setSource(nullptr);
}

/*
 * SOURCE MODIFERS
 */

template <typename T>
inline void Consumer<T>::setSource(Source<T>* src){
	std::lock_guard<std::mutex> lock(m_mutex);

	//Detach consumer from the previous source
	if(m_source)
		m_source->detach(this);

	//Attach the new consumer
	m_source=src;
	if(m_source)
		m_source->attach(this);
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(Source<T>& src){
	setSource(&src);
	return *this;
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(std::nullptr_t ptr){
	setSource(nullptr);
	return *this;
}

template <typename T>
inline std::shared_ptr<const T>	Consumer<T>::get(){
	if(m_source)
		return m_source->get();
	else
		return std::shared_ptr<const T>(); //No source, return an empty ptr
}

} /* namespace Zuazo */
