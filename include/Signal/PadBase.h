#pragma once

#include <string>
#include <typeindex>

#include "../Utils/Macros.h"

namespace Zuazo::Signal {

class Layout;

class PadBase {
    friend Layout;
public:
    enum Direction {
        NONE    = 0,
        INPUT   = ZUAZO_BIT(0),
        OUTPUT  = ZUAZO_BIT(1)
    };

    PadBase(const std::type_index& type, Direction dir = Direction::NONE, std::string&& name = ""); 
    virtual ~PadBase();

    const Layout*       getOwner() const;
    Layout*             getOwner();

    const std::type_index& getType() const;
    Direction           getDirection() const;
    const std::string&  getName() const;

protected:
    PadBase(const PadBase& other); 
    PadBase&            operator=(const PadBase& other);

    void                setType(const std::type_info& type);
    void                setDirection(Direction dir);
    void                setName(std::string&& name);

private:
    Layout*             m_owner = nullptr;

    std::type_index     m_type;
    Direction           m_direction;
    std::string         m_name;
    
};

ZUAZO_ENUM_FLAG_OPERATORS(PadBase::Direction)

}

#include "PadBase.inl"