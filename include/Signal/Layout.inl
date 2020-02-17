#include "Layout.h"

#include "../Exception.h"

#include <utility>

namespace Zuazo::Signal {

/*
 * Layout
 */

inline Layout::Layout(std::string&& name) 
	: m_name(std::move(name))
{
}

inline Layout::Layout(Layout&& other) 
	: m_name(std::move(other.m_name))
	, m_pads(std::move(other.m_pads))
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

	return *this;
}

inline void Layout::setName(std::string&& name){
	m_name = std::move(name);
}

inline const std::string& Layout::getName() const{
	return m_name;
}

inline const std::set<Layout::PadBase*>& Layout::getPads(){
	return m_pads;
}

inline std::set<const Layout::PadBase*>  Layout::getPads() const{
	return std::set<const PadBase*>(m_pads.cbegin(), m_pads.cend());
}

template<typename T>
inline Layout::OutputPad<T>& Layout::getOutput(const std::string_view& str){
	return findOutput<T>(str);
}

template<typename T>
inline const Layout::OutputPad<T>& Layout::getOutput(const std::string_view& str) const{
	return findOutput<T>(str);
}

template<typename T>
inline Layout::InputPad<T>& Layout::getInput(const std::string_view& str){
	return findInput<T>(str);
}

template<typename T>
inline const Layout::InputPad<T>& Layout::getInput(const std::string_view& str) const{
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

inline Layout::PadBase& Layout::findPad(const std::string_view& name, 
										PadBase::Direction  dir, 
										const std::type_info& type ) const
{
	for(auto pad : m_pads) {
		if( pad->getName() == name &&
			pad->getDirection() == dir && 
			pad->getType() == type )
		{
			return *pad;
		}
	}

	throw Exception("Requested pad not found");
}

template<typename T>
inline Layout::OutputPad<T>& Layout::findOutput(const std::string_view& name) const{
	return dynamic_cast<OutputPad<T>&>( findPad(
		name,
		PadBase::Direction::OUTPUT,
		typeid(T)
	));
}

template<typename T>
inline Layout::InputPad<T>& Layout::findInput(const std::string_view& name) const{
	return dynamic_cast<InputPad<T>&>( findPad(
		name,
		PadBase::Direction::INPUT,
		typeid(T)
	));
}

/*
 * PadBase
 */

inline const Layout* Layout::PadBase::getOwner() const{
	return m_owner;
}

inline Layout* Layout::PadBase::getOwner(){
	return m_owner;
}



inline const std::type_index& Layout::PadBase::getType() const{
	return m_type;
}

inline const std::string& Layout::PadBase::getName() const{
	return m_name;
}

inline Layout::PadBase::Direction Layout::PadBase::getDirection() const{
	return m_direction;
}



inline Layout::PadBase::PadBase(const std::type_index& type, 
						Direction dir, 
						std::string&& name,
						Layout* owner )
	: m_type(type)
	, m_direction(dir)
	, m_name(std::move(name))
{
	if(owner) owner->addPad(*this);
}

inline Layout::PadBase::PadBase(const PadBase& other) :
	m_type(other.m_type),
	m_direction(other.m_direction),
	m_name(other.m_name)
{
	if(other.m_owner) other.m_owner->addPad(*this);
}

inline Layout::PadBase::PadBase(PadBase&& other) :
	m_type(std::move(other.m_type)),
	m_direction(std::move(other.m_direction)),
	m_name(std::move(other.m_name))
{
	//Steal its owner
	if(other.m_owner){ 
		other.m_owner->addPad(*this);
		other.m_owner->removePad(other);
	}
}

inline Layout::PadBase::~PadBase() {
	if(m_owner) m_owner->removePad(*this);
}


inline Layout::PadBase& Layout::PadBase::operator=(const PadBase& other){
	m_type = other.m_type;
	m_direction = other.m_direction;
	m_name = other.m_name;

	//Set the new owner
	if(m_owner) m_owner->removePad(*this);
	if(other.m_owner) other.m_owner->addPad(*this);

	return *this;
}

inline Layout::PadBase& Layout::PadBase::operator=(PadBase&& other){
	m_type = std::move(other.m_type);
	m_direction = std::move(other.m_direction);
	m_name = std::move(other.m_name);

	//Reset my owner
	if(m_owner) m_owner->removePad(*this);

	//Steal its owner
	if(other.m_owner){ 
		other.m_owner->addPad(*this);
		other.m_owner->removePad(other);
	}

	return *this;
}




inline void Layout::PadBase::setType(const std::type_info& type) {
	m_type = type;
}

inline void Layout::PadBase::setDirection(Direction dir) {
	m_direction = dir;
}

inline void Layout::PadBase::setName(std::string&& name){
	m_name = std::move(name);
}

/*
 * InputPad
 */

template <typename T>
inline const T Layout::InputPad<T>::NO_SIGNAL = T();

template <typename T>
inline void Layout::InputPad<T>::setSource(OutputPad<T>* src){
	if(m_source) m_source->m_consumers.erase(this);
	m_source = src;
	if(m_source) m_source->m_consumers.emplace(this);
}

template <typename T>
inline Layout::OutputPad<T>* Layout::InputPad<T>::getSource() const{
	return m_source;
}



template <typename T>
inline Layout::InputPad<T>::InputPad(const InputPad& other) :
	PadBase(other)
{
	setSource(other.m_source);
}

template <typename T>
inline Layout::InputPad<T>::InputPad(InputPad&& other) :
	PadBase(other)
{
	setSource(other.m_source);
	other.setSource(nullptr);
}

template <typename T>
inline Layout::InputPad<T>::~InputPad(){
	setSource(nullptr);
}




template <typename T>
Layout::InputPad<T>& Layout::InputPad<T>::operator=(const InputPad& other){
	static_cast<PadBase&>(this) = other;
	setSource(other.m_source);

	return *this;
}

template <typename T>
Layout::InputPad<T>& Layout::InputPad<T>::operator=(InputPad&& other){
	static_cast<PadBase&>(this) = std::move(other);
	setSource(other.m_source);
	other.setSource(nullptr);

	return *this;
}

template <typename T>
inline const T& Layout::InputPad<T>::get() const{
	if(m_source) {
		return m_source->get();
	} else {
		return NO_SIGNAL;
	}
}

/*
 * OutputPad
 */

template <typename T>
inline const std::set<Layout::InputPad<T>*>& Layout::OutputPad<T>::getConsumers() const{
	return m_consumers;
}

template <typename T>
inline Layout::OutputPad<T>::OutputPad(OutputPad&& other) :
	PadBase(std::move(other)),
	m_consumers(std::move(other.m_consumers))
{
	//Steal it's consumers
	for(auto cons : m_consumers){
		cons->m_source = this;
	}
}

template <typename T>
inline Layout::OutputPad<T>::~OutputPad(){
	for(auto cons : m_consumers){
		cons->m_source = nullptr;
	}
}


template <typename T>
inline Layout::OutputPad<T>& Layout::OutputPad<T>::operator=(OutputPad&& other){
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

/*
 * operator<<
 */

template <typename T>
inline void operator<<(Layout::InputPad<T>& dst, Layout::OutputPad<T>& src){
	dst.setSource(&src);
}

template <typename T>
inline void operator<<(Layout::InputPad<T>& dst, NoSignal){
	dst.setSource(nullptr);
}

}