#include "Layout.h"

#include <utility>

namespace Zuazo::Signal {

inline Layout::Layout(std::string&& name) :
	m_name(std::forward<std::string>(name))
{
}

inline Layout::Layout(Layout&& other) :
	m_name(std::move(other.m_name)),
	m_pads(std::move(other.m_pads))
{
	for(auto pad : m_pads){
		pad->m_owner = this;
	}
}

inline Layout::~Layout() {
	for(auto pad : m_pads){
		pad->m_owner = nullptr;
	}
}

inline Layout& Layout::operator=(Layout&& other){
	m_name = std::move(other.m_name);

	//Remove all my pads
	for(auto pad : m_pads){
		pad->m_owner = nullptr;
	}

	//Steal all its pads
	m_pads = std::move(other.m_pads);
	for(auto pad : m_pads){
		pad->m_owner = this;
	}
}

inline void Layout::setName(std::string&& name){
	m_name = std::forward<std::string>(name);
}

inline const std::string& Layout::getName() const{
	return m_name;
}

inline const std::set<PadBase*>& Layout::getPads(){
	return m_pads;
}

inline std::set<const PadBase*>  Layout::getPads() const{
	return std::set<const PadBase*>(m_pads.cbegin(), m_pads.cend());
}

template<typename T>
inline OutputPad<T>* Layout::getOutput(const std::string& str){
	return findOutput<T>(str);
}

template<typename T>
inline const OutputPad<T>* Layout::getOutput(const std::string& str) const{
	return findOutput<T>(str);
}

template<typename T>
inline InputPad<T>* Layout::getInput(const std::string& str){
	return findInput<T>(str);
}

template<typename T>
inline const InputPad<T>* Layout::getInput(const std::string& str) const{
	return findInput<T>(str);
}              

inline void Layout::addPad(PadBase& pad){
	if(pad.m_owner == nullptr){ //Pad can't have an owner
		pad.m_owner = this; //Make me its owner
		m_pads.emplace(&pad); //Add it to my list
	}
}

inline void Layout::removePad(PadBase& pad){
	if(pad.m_owner == this){ //I must be its owner
		pad.m_owner = nullptr;
		m_pads.erase(&pad); //Remove it from my list
	}
}

template<typename T>
OutputPad<T>* Layout::findOutput(const std::string& name) const{
	for(auto pad : m_pads) {
		if( (pad->getDirection() == PadBase::Direction::OUTPUT) && 
			pad->getName() == name &&
			pad->getType() == typeid(T)
		){
			return dynamic_cast<OutputPad<T>*>(pad);
		}
	}

	return nullptr;
}

template<typename T>
InputPad<T>* Layout::findInput(const std::string& name) const{
	for(auto pad : m_pads) {
		if( (pad->getDirection() == PadBase::Direction::INPUT) && 
			pad->getName() == name &&
			pad->getType() == typeid(T)
		){
			return dynamic_cast<InputPad<T>*>(pad);
		}
	}

	return nullptr;
}

}