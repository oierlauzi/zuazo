#pragma once

#include <memory>
#include <mutex>
#include <set>

#include "../Utils/Rational.h"
#include "../Timing.h"
#include "Consumer.h"
#include "StreamElement.h"
#include "Operators.h"

namespace Zuazo::Stream{

template <typename T>
class Consumer;

template <typename T>
class Source : public Timing{
	friend Consumer<T>& operator<< <>(Consumer<T>& cons,  Source<T>& src);
	friend Consumer<T>& operator<< <>(Consumer<T>& cons,  Source<T>* src);
	friend Source<T>& operator>> <>(Source<T>& src,  Consumer<T>& cons);
	friend Consumer<T>;
public:
	Source();
	Source(const Rational& rate);
	Source(const Source<T>& other)=delete;
	~Source();
protected:
	void									push(std::unique_ptr<T>& newElement);
	bool									isInUse();
private:
	std::shared_ptr<const StreamElement<T>>	m_lastElement;

	std::set<Consumer<T>*> 					m_consumers;
};

/*
 * FUNCTION DEFINITIONS
 */

template <typename T>
inline Source<T>::Source() : Timing(){

}

template <typename T>
inline Source<T>::Source(const Rational& rate) : Timing(rate){

}

template <typename T>
inline Source<T>::~Source(){

}

template <typename T>
inline void	Source<T>::push(std::unique_ptr<T>& newElement){
	m_lastElement=std::shared_ptr<const StreamElement<T>>(new StreamElement<T>(newElement) );

	//Call all the consumers
	for(Consumer<T> * cons : m_consumers){
		cons->push(m_lastElement);
	}
}

template <typename T>
inline bool Source<T>::isInUse(){
	for(Consumer<T> * cons : m_consumers)
		if(cons->isActive())
			return true;
	return false;
}
}
