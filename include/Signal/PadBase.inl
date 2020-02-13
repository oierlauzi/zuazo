#include "PadBase.h"

#include "Layout.h"

#include <utility>

namespace Zuazo::Signal {

inline PadBase::PadBase(const std::type_index& type, Direction dir, std::string&& name) :
	m_type(type),
	m_direction(dir),
	m_name(std::move(name))
{
}

inline PadBase::~PadBase() {
	if(m_owner) m_owner->removePad(*this);
}



inline const Layout* PadBase::getOwner() const{
	return m_owner;
}

inline Layout* PadBase::getOwner(){
	return m_owner;
}



inline const std::type_index& PadBase::getType() const{
	return m_type;
}

inline const std::string& PadBase::getName() const{
	return m_name;
}

inline PadBase::Direction PadBase::getDirection() const{
	return m_direction;
}



inline PadBase::PadBase(const PadBase& other) :
	m_type(other.m_type),
	m_direction(other.m_direction),
	m_name(other.m_name)
{
	if(other.m_owner) other.m_owner->addPad(*this);
}

inline PadBase::PadBase(PadBase&& other) :
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

inline PadBase& PadBase::operator=(const PadBase& other){
	m_type = other.m_type;
	m_direction = other.m_direction;
	m_name = other.m_name;

	//Set the new owner
	if(m_owner) m_owner->removePad(*this);
	if(other.m_owner) other.m_owner->addPad(*this);

	return *this;
}

inline PadBase& PadBase::operator=(PadBase&& other){
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




inline void PadBase::setType(const std::type_info& type) {
	m_type = type;
}

inline void PadBase::setDirection(Direction dir) {
	m_direction = dir;
}

inline void PadBase::setName(std::string&& name){
	m_name = std::move(name);
}

}