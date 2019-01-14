#pragma once

#include <stddef.h>
#include <memory>
#include <mutex>

#include "../Utils/Rational.h"
#include "../Timing.h"
#include "Source.h"
#include "StreamElement.h"
#include "Operators.h"

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer : public Timing{
	friend Consumer<T>& operator<< <>(Consumer<T>& cons,  Source<T>& src);
	friend Consumer<T>& operator<< <>(Consumer<T>& cons,  Source<T>* src);
	friend Source<T>& operator>> <>(Source<T>& src,  Consumer<T>& cons);
	friend Source<T>;
public:
	Consumer();
	Consumer(const Rational& rate);
	Consumer(const Consumer<T>& other);
	~Consumer();

	void									setSource(Source<T>* src=NULL);

	virtual bool							isActive();
protected:
	std::shared_ptr<const StreamElement<T>> get();
private:
	Source<T>*									m_source;
	std::shared_ptr<const StreamElement<T>> m_lastElement;

	virtual void							push(std::shared_ptr<const StreamElement<T>> element);
};

/*
 * FUNCTION DEFINITIONS
 */

template <typename T>
inline Consumer<T>::Consumer(){
	m_source=NULL;
}

template <typename T>
inline Consumer<T>::Consumer(const Rational& rate) : Timing(rate){
	m_source=NULL;
}

template <typename T>
inline Consumer<T>::Consumer(const Consumer<T>& other) : Consumer(other.getRate()) {
	setSource(other.m_source);
}

template <typename T>
inline Consumer<T>::~Consumer(){
	setSource();
}

template <typename T>
inline void Consumer<T>::setSource(Source<T> * src){
	std::unique_lock<std::mutex> lock(m_mutex);
	//Release the old source
	if(m_source){
		std::unique_lock<std::mutex> lock(m_source->m_mutex);
		m_source->m_consumers.erase(this);
	}

	//Set the new source
	m_source=src;
	if(m_source){
		std::unique_lock<std::mutex> lock(m_source->m_mutex);
		m_lastElement=m_source->m_lastElement;
		m_source->m_consumers.insert(this);
	}
}

template <typename T>
inline bool Consumer<T>::isActive(){
	return true; //Method should be modified by child classes
}

template <typename T>
inline std::shared_ptr<const StreamElement<T>> Consumer<T>::get(){
	return m_lastElement;
}

template <typename T>
void Consumer<T>::push(std::shared_ptr<const StreamElement<T>> element){
	std::unique_lock<std::mutex> lock(m_mutex);
	m_lastElement=element;
}
}
