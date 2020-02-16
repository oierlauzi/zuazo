#pragma once

#include <string>
#include <typeindex>

namespace Zuazo::Signal {

class Layout;

class PadBase {
	friend Layout;
public:
	enum Direction {
		INPUT,
		OUTPUT
	};

	const Layout*       getOwner() const;
	Layout*             getOwner();

	const std::type_index& getType() const;
	Direction           getDirection() const;
	const std::string&  getName() const;

protected:
	PadBase(	const std::type_index& type, 
				Direction dir, 
				std::string&& name, 
				Layout* owner ); 
	PadBase(const PadBase& other); 
	PadBase(PadBase&& other);
	virtual ~PadBase();

	PadBase&            operator=(const PadBase& other);
	PadBase&            operator=(PadBase&& other);

	void                setType(const std::type_info& type);
	void                setDirection(Direction dir);
	void                setName(std::string&& name);

private:
	Layout*             m_owner = nullptr;

	std::type_index     m_type;
	Direction           m_direction;
	std::string         m_name;
	
};

}

#include "PadBase.inl"