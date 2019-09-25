#pragma once

#include <string>
#include <typeindex>
#include <typeinfo>

#include "../Utils/Macros.h"

namespace Zuazo::Signal {

class Layout;

class PadBase {
    friend Layout;
public:
    enum Direction {
        INPUT   = ZUAZO_BIT(0),
        OUTPUT  = ZUAZO_BIT(1)
    };

    PadBase(std::string&& name, Direction dir, const std::type_info& type); 
    PadBase(const PadBase& other) = delete; 
    virtual ~PadBase();

    const Layout*       getOwner() const;
    Layout*             getOwner();

    const std::string&  getName() const;
    Direction           getDirection() const;
    const std::type_index& getType() const;

protected:
    void                setName(std::string&& name);
    void                setDirection(Direction dir);
    void                setType(const std::type_info& type);

private:
    Layout*             m_owner = nullptr;

    std::string         m_name;
    Direction           m_direction;
    std::type_index     m_type;
};

ZUAZO_ENUM_FLAG_OPERATORS(PadBase::Direction)

}

#include "PadBase.inl"