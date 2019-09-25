#include "PadBase.h"

#include "Layout.h"

#include <utility>

namespace Zuazo::Signal {

inline PadBase::PadBase(std::string&& name, Direction dir, const std::type_info& type) :
    m_name(std::forward<std::string>(name)),
    m_direction(dir),
    m_type(type)
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

inline const std::string& PadBase::getName() const{
    return m_name;
}

inline PadBase::Direction PadBase::getDirection() const{
    return m_direction;
}

inline const std::type_index& PadBase::getType() const{
    return m_type;
}

inline void PadBase::setName(std::string&& name){
    m_name = std::forward<std::string>(name);
}

inline void PadBase::setDirection(Direction dir) {
    m_direction = dir;
}

inline void PadBase::setType(const std::type_info& type) {
    m_type = type;
}

}