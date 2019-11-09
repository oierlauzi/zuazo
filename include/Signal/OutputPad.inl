#include "OutputPad.h"

#include "InputPad.h"

#include <utility>

namespace Zuazo::Signal {

template <typename T>
inline OutputPad<T>::OutputPad(std::string&& name) : 
	PadBase(typeid(T), PadBase::Direction::OUTPUT, std::move(name))
{
}

template <typename T>
inline OutputPad<T>::~OutputPad(){
	for(auto cons : m_consumers){
		cons->m_source = nullptr;
	}
}

template <typename T>
inline const std::set<InputPad<T>*>& OutputPad<T>::getConsumers() const{
	return m_consumers;
}

template <typename T>
OutputPad<T>::OutputPad(OutputPad&& other) :
	PadBase(std::move(other)),
	m_consumers(std::move(other.m_consumers))
{
	//Steal it's consumers
	for(auto cons : m_consumers){
		cons->m_source = this;
	}
}

template <typename T>
OutputPad<T>& OutputPad<T>::operator=(OutputPad&& other){
	static_cast<PadBase&>(*this) = std::move(other);

	for(auto cons : m_consumers){
		cons->m_source = nullptr;
	}

	//Steal it's consumers
	m_consumers = std::move(other.m_consumers); 
	for(auto cons : m_consumers){
		cons->m_source = this;
	}

	return *this;
}

}
